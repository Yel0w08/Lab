#include "Manifest.h"

#include <QList>
#include <QMap>
#include <QPair>
#include <memory>
#include <utility>

// ---------------------------------------------------------------------------
// Minimal YAML subset parser.
//
// YLEDMETA.yaml only ever uses nested mappings of scalars, so a tiny
// indentation-based parser is enough. Values are flattened to dotted paths
// (e.g. "dl.link-tgz.linux") and then looked up by key name, which makes the
// lookups tolerant of the uneven indentation people write by hand.
// ---------------------------------------------------------------------------

namespace {

struct YNode
{
    bool isMap = true;
    QString value;
    QMap<QString, std::shared_ptr<YNode>> children;
};

void flatten(const YNode *n, const QString &prefix, QList<QPair<QString, QString>> &out)
{
    if (!n)
        return;

    if (!n->isMap) {
        out.append({ prefix.toLower(), n->value });
        return;
    }

    for (auto it = n->children.cbegin(); it != n->children.cend(); ++it) {
        const QString p = prefix.isEmpty()
            ? it.key()
            : prefix + QLatin1Char('.') + it.key();
        flatten(it.value().get(), p, out);
    }
}

QString unquote(QString s)
{
    s = s.trimmed();
    if (s.size() >= 2) {
        const QChar a = s.front();
        const QChar b = s.back();
        if ((a == QLatin1Char('"') && b == QLatin1Char('"')) ||
            (a == QLatin1Char('\'') && b == QLatin1Char('\'')))
            s = s.mid(1, s.size() - 2);
    }
    return s;
}

bool asBool(const QString &v)
{
    const QString s = v.trimmed().toLower();
    return s == QLatin1String("true") || s == QLatin1String("yes") ||
           s == QLatin1String("1")    || s == QLatin1String("on");
}

} // namespace

Manifest Manifest::parse(const QString &yamlText)
{
    Manifest m;
    m.raw = yamlText;

    auto root = std::make_unique<YNode>();

    struct Frame { int indent; YNode *node; };
    QList<Frame> stack;
    stack.append({ -1, root.get() });

    const QStringList lines = yamlText.split(QLatin1Char('\n'));
    for (QString line : lines) {
        line.remove(QLatin1Char('\r'));
        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith(QLatin1Char('#')))
            continue;

        int indent = 0;
        while (indent < line.size() &&
               (line.at(indent) == QLatin1Char(' ') || line.at(indent) == QLatin1Char('\t')))
            ++indent;

        const QString content = line.mid(indent);
        const int colon = content.indexOf(QLatin1Char(':'));
        if (colon < 0)
            continue;

        const QString key = content.left(colon).trimmed();
        const QString rest = content.mid(colon + 1).trimmed();
        if (key.isEmpty())
            continue;

        while (stack.size() > 1 && stack.last().indent >= indent)
            stack.removeLast();

        YNode *parent = stack.last().node;

        auto child = std::make_shared<YNode>();
        if (rest.isEmpty()) {
            child->isMap = true;
        } else {
            child->isMap = false;
            child->value = unquote(rest);
        }

        YNode *childPtr = child.get();
        parent->children.insert(key.toLower(), child);

        if (childPtr->isMap)
            stack.append({ indent, childPtr });
    }

    QList<QPair<QString, QString>> leaves;
    flatten(root.get(), QString(), leaves);

    // ---- lookups -----------------------------------------------------------

    auto valueAt = [&](const QString &section, const QString &key) -> QString {
        const QString want = section.toLower() + QLatin1Char('.') + key.toLower();
        for (const auto &pr : leaves)
            if (pr.first == want)
                return pr.second;
        return {};
    };

    // First leaf whose last dotted segment equals `leafName`.
    auto findLeaf = [&](const QString &leafName) -> QString {
        for (const auto &pr : leaves) {
            const QStringList segs = pr.first.split(QLatin1Char('.'));
            if (!segs.isEmpty() && segs.last() == leafName)
                return pr.second;
        }
        return {};
    };

    // First leaf under a parent path containing `family` whose last segment is
    // `platform`, falling back to an "all" entry.
    auto findUrl = [&](const QString &family, const QString &platform) -> QString {
        QString fallback;
        for (const auto &pr : leaves) {
            const QStringList segs = pr.first.split(QLatin1Char('.'));
            if (segs.size() < 2)
                continue;

            const QString last = segs.last();
            const QString parentPath = pr.first.left(pr.first.size() - last.size() - 1);
            if (!parentPath.contains(family))
                continue;

            if (last == platform)
                return pr.second;
            if (last == QLatin1String("all") && fallback.isEmpty())
                fallback = pr.second;
        }
        return fallback;
    };

    // Accept common platform aliases so manifests can use windows/win,
    // linux/unix and macos/mac/osx/darwin interchangeably.
    auto valueAtAny = [&](const QString &section, const QStringList &keys) -> QString {
        for (const QString &k : keys) {
            const QString v = valueAt(section, k);
            if (!v.isEmpty())
                return v;
        }
        return {};
    };
    auto findUrlAny = [&](const QString &family, const QStringList &keys) -> QString {
        for (const QString &k : keys) {
            const QString v = findUrl(family, k);
            if (!v.isEmpty())
                return v;
        }
        return {};
    };

    m.runWindows = valueAtAny(QStringLiteral("run"),
                              { QStringLiteral("windows"), QStringLiteral("win") });
    m.runLinux   = valueAtAny(QStringLiteral("run"),
                              { QStringLiteral("linux"), QStringLiteral("unix") });
    m.runMac     = valueAtAny(QStringLiteral("run"),
                              { QStringLiteral("macos"), QStringLiteral("mac"),
                                QStringLiteral("osx"), QStringLiteral("darwin") });

    m.initWindows = valueAtAny(QStringLiteral("init"),
                               { QStringLiteral("windows"), QStringLiteral("win") });
    m.initLinux   = valueAtAny(QStringLiteral("init"),
                               { QStringLiteral("linux"), QStringLiteral("unix") });
    m.initMac     = valueAtAny(QStringLiteral("init"),
                               { QStringLiteral("macos"), QStringLiteral("mac"),
                                 QStringLiteral("osx"), QStringLiteral("darwin") });

    m.useTgz    = asBool(findLeaf(QStringLiteral("usetgz")));
    m.useGitDir = asBool(findLeaf(QStringLiteral("usegitdir")));

    const QStringList winKeys   = { QStringLiteral("windows"), QStringLiteral("win") };
    const QStringList linKeys   = { QStringLiteral("linux"), QStringLiteral("unix") };
    const QStringList macKeys   = { QStringLiteral("macos"), QStringLiteral("mac"),
                                    QStringLiteral("osx"), QStringLiteral("darwin") };

    m.tgzWindows = findUrlAny(QStringLiteral("tgz"), winKeys);
    m.tgzLinux   = findUrlAny(QStringLiteral("tgz"), linKeys);
    m.tgzMac     = findUrlAny(QStringLiteral("tgz"), macKeys);

    m.gitDirWindows = findUrlAny(QStringLiteral("git"), winKeys);
    m.gitDirLinux   = findUrlAny(QStringLiteral("git"), linKeys);
    m.gitDirMac     = findUrlAny(QStringLiteral("git"), macKeys);
    m.gitDirAll     = findUrl(QStringLiteral("git"), QStringLiteral("all"));

    // Tolerate hand-written manifests: fall back to any value that looks like a
    // tarball link or a GitHub tree/folder link.
    if (m.tgzLinux.isEmpty() && m.tgzWindows.isEmpty() && m.tgzMac.isEmpty()) {
        for (const auto &pr : leaves) {
            if (!pr.second.endsWith(QLatin1String(".tar.gz")) &&
                !pr.second.endsWith(QLatin1String(".tgz")))
                continue;
            const QStringList segs = pr.first.split(QLatin1Char('.'));
            const QString last = segs.isEmpty() ? QString() : segs.last();
            if (last == QLatin1String("windows"))      m.tgzWindows = pr.second;
            else if (last == QLatin1String("macos"))   m.tgzMac = pr.second;
            else                                       m.tgzLinux = pr.second;
        }
    }
    if (m.gitDirLinux.isEmpty() && m.gitDirWindows.isEmpty() &&
        m.gitDirMac.isEmpty() && m.gitDirAll.isEmpty()) {
        for (const auto &pr : leaves) {
            if (!pr.second.contains(QLatin1String("/tree/")))
                continue;
            const QStringList segs = pr.first.split(QLatin1Char('.'));
            const QString last = segs.isEmpty() ? QString() : segs.last();
            if (last == QLatin1String("windows"))      m.gitDirWindows = pr.second;
            else if (last == QLatin1String("macos"))   m.gitDirMac = pr.second;
            else if (last == QLatin1String("all"))     m.gitDirAll = pr.second;
            else                                       m.gitDirLinux = pr.second;
        }
    }

    m.valid = !yamlText.trimmed().isEmpty();
    return m;
}

QString Manifest::platform()
{
#if defined(Q_OS_WIN)
    return QStringLiteral("windows");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("macos");
#else
    return QStringLiteral("linux");
#endif
}

bool Manifest::hasDownload() const
{
    return !tgzUrl().isEmpty() || !gitDirUrl().isEmpty();
}

QString Manifest::runCommand() const
{
    const QString p = platform();
    if (p == QLatin1String("windows")) return runWindows;
    if (p == QLatin1String("macos"))   return runMac;
    return runLinux;
}

QString Manifest::initCommand() const
{
    const QString p = platform();
    if (p == QLatin1String("windows")) return initWindows;
    if (p == QLatin1String("macos"))   return initMac;
    return initLinux;
}

QString Manifest::tgzUrl() const
{
    const QString p = platform();
    if (p == QLatin1String("windows")) return tgzWindows;
    if (p == QLatin1String("macos"))   return tgzMac;
    return tgzLinux;
}

QString Manifest::gitDirUrl() const
{
    const QString p = platform();
    if (p == QLatin1String("windows") && !gitDirWindows.isEmpty()) return gitDirWindows;
    if (p == QLatin1String("macos")   && !gitDirMac.isEmpty())     return gitDirMac;
    if (p == QLatin1String("linux")   && !gitDirLinux.isEmpty())   return gitDirLinux;
    return gitDirAll;
}
