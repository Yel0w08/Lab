#include "ExperimentService.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>

namespace {

const QString kDbUrl =
    QStringLiteral("https://raw.githubusercontent.com/Yel0w08/Lab/main/LabManager.db");

// Legacy fallback package store (used when a manifest declares no download).
const QString kStorageBaseUrl =
    QStringLiteral("https://github.com/Yel0w08/storage.archive.data/raw/refs/heads/main/Yel0w.Lab/");

// Where YLEDMETA.yaml lives. The Lab repo is the source of truth; the BINDT
// repo is kept as a fallback for experiments whose metadata only lives there.
const QStringList kManifestBaseUrls = {
    QStringLiteral("https://raw.githubusercontent.com/Yel0w08/Lab/main/Experiments/"),
    QStringLiteral("https://raw.githubusercontent.com/Yel0w08/BINDT/main/LAB/EXPERIMENTS/")
};

const int kCacheDurationMinutes = 5;

QDateTime parseDateTime(const QString &s)
{
    if (s.isEmpty())
        return {};

    // Some rows store a long fractional part; chop to seconds if needed.
    if (s.size() >= 19) {
        const QDateTime dt = QDateTime::fromString(s.left(19), QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        if (dt.isValid())
            return dt;
    }

    const QDateTime iso = QDateTime::fromString(s, Qt::ISODate);
    if (iso.isValid())
        return iso;

    bool ok = false;
    const qlonglong secs = s.toLongLong(&ok);
    if (ok)
        return QDateTime::fromSecsSinceEpoch(secs);

    return {};
}

void copyDirRecursive(const QString &src, const QString &dst)
{
    const QDir s(src);
    if (!s.exists())
        return;

    QDir().mkpath(dst);
    const QDir d(dst);

    const QStringList entries = s.entryList(QDir::Files | QDir::Dirs |
                                            QDir::NoDotAndDotDot | QDir::Hidden |
                                            QDir::System);
    for (const QString &entry : entries) {
        const QString sp = s.filePath(entry);
        const QString dp = d.filePath(entry);
        if (QFileInfo(sp).isDir()) {
            copyDirRecursive(sp, dp);
        } else {
            QFile::remove(dp);
            if (QFile::copy(sp, dp))
                QFile::setPermissions(dp, QFile::permissions(sp));
        }
    }
}

void removeDir(const QString &path)
{
    QDir d(path);
    if (d.exists())
        d.removeRecursively();
}

// If an archive unpacked into a single folder named after the project, move its
// contents up one level.
void flattenNested(const QString &targetDir, const QString &projectPath)
{
    const QDir base(targetDir);
    QDir nested(targetDir + QLatin1Char('/') + projectPath);
    if (!nested.exists() || nested.absolutePath() == base.absolutePath())
        return;

    const QStringList files = nested.entryList(QDir::Files);
    for (const QString &file : files)
        QFile::rename(nested.filePath(file), base.filePath(file));

    const QStringList dirs = nested.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &d : dirs)
        QFile::rename(nested.filePath(d), base.filePath(d));

    nested.removeRecursively();
}

// Run a shell command and wait (bounded) for it. Used for the manifest `init`.
int runShellBlocking(const QString &cmd, const QString &dir)
{
    if (cmd.trimmed().isEmpty())
        return 0;

    QProcess p;
    p.setWorkingDirectory(dir);
#ifdef Q_OS_WIN
    p.start(QStringLiteral("cmd"), { QStringLiteral("/c"), cmd });
#else
    p.start(QStringLiteral("bash"), { QStringLiteral("-c"), cmd });
#endif
    if (!p.waitForStarted(5000))
        return -1;
    p.waitForFinished(300000);
    return p.exitCode();
}

// A bare "./script.sh" (or "script.sh") run command — used to make the script
// executable before launching it.
QString extractScriptName(const QString &cmd)
{
    const QString c = cmd.trimmed();
    if (c.isEmpty() || c.contains(QLatin1Char(' ')))
        return {};
    QString s = c;
    if (s.startsWith(QLatin1String("./")))
        s = s.mid(2);
    return s.endsWith(QLatin1String(".sh")) ? s : QString();
}

bool parseGithubTreeUrl(const QString &url, QString *owner, QString *repo,
                        QString *branch, QString *subPath)
{
    const QUrl u(url);
    if (u.host() != QLatin1String("github.com"))
        return false;

    const QStringList segs = u.path().split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if (segs.size() < 4 || segs.at(2) != QLatin1String("tree"))
        return false;

    *owner   = segs.at(0);
    *repo    = segs.at(1);
    *branch  = segs.at(3);
    *subPath = segs.mid(4).join(QLatin1Char('/'));
    return true;
}

} // namespace

ExperimentService::ExperimentService(QObject *parent)
    : QObject(parent)
{
    m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(m_cacheDir);
    m_dbPath = m_cacheDir + QStringLiteral("/LabManager.db");
}

QString ExperimentService::experimentsRoot() const
{
    const QString root = m_appDataDir + QStringLiteral("/Experiments");
    QDir().mkpath(root);
    return root;
}

void ExperimentService::requestExperiments(bool force)
{
    // Serve from the in-memory cache when it is still fresh.
    if (!force && !m_experiments.isEmpty() && m_lastRefresh.isValid() &&
        m_lastRefresh.secsTo(QDateTime::currentDateTime()) < kCacheDurationMinutes * 60) {
        emit experimentsChanged(true, QString());
        return;
    }

    if (m_dbPending || m_downloading)
        return;

    if (QFile::exists(m_dbPath)) {
        loadFromDatabase();
        return;
    }

    downloadDatabase();
}

void ExperimentService::downloadDatabase()
{
    m_dbPending = true;

    QNetworkRequest req{QUrl(kDbUrl)};

    QNetworkReply *reply = m_net.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_dbPending = false;

        if (reply->error() != QNetworkReply::NoError) {
            const QString err = reply->errorString();
            reply->deleteLater();
            emit experimentsChanged(false,
                tr("Failed to download database: %1").arg(err));
            return;
        }

        const QByteArray data = reply->readAll();
        reply->deleteLater();

        QFile f(m_dbPath);
        if (!f.open(QIODevice::WriteOnly)) {
            const QString err = f.errorString();
            emit experimentsChanged(false,
                tr("Failed to write database cache: %1").arg(err));
            return;
        }
        f.write(data);
        f.close();

        loadFromDatabase();
    });
}

void ExperimentService::loadFromDatabase()
{
    if (!QFile::exists(m_dbPath)) {
        downloadDatabase();
        return;
    }

    const QString connName = QStringLiteral("yled_experiments");
    if (QSqlDatabase::contains(connName))
        QSqlDatabase::removeDatabase(connName);

    QList<Experiment> loaded;
    QString error;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connName);
        db.setDatabaseName(m_dbPath);

        if (!db.open()) {
            error = tr("Failed to open database: %1").arg(db.lastError().text());
            db.close();
            QSqlDatabase::removeDatabase(connName);
            emit experimentsChanged(false, error);
            return;
        }

        QSqlQuery q(db);
        q.prepare(QStringLiteral(
            "SELECT Id, Name, Description, Language, Framework, Engine, Status, "
            "ProjectPath, Tags, Favorite, Downloadable, Notes, CreatedAt, LastModified "
            "FROM Experiments ORDER BY Favorite DESC, LastModified DESC"));

        if (!q.exec()) {
            error = tr("Failed to read experiments: %1").arg(q.lastError().text());
            db.close();
            QSqlDatabase::removeDatabase(connName);
            emit experimentsChanged(false, error);
            return;
        }

        while (q.next()) {
            Experiment e;
            e.id            = q.value(0).toInt();
            e.name          = q.value(1).toString();
            e.description   = q.value(2).toString();
            e.language      = q.value(3).toString();
            e.framework     = q.value(4).toString();
            e.engine        = q.value(5).toString();
            e.status        = q.value(6).toString();
            e.projectPath   = q.value(7).toString();
            const QString tags = q.value(8).toString();
            const QStringList parts = tags.split(QLatin1Char(','), Qt::SkipEmptyParts);
            for (const QString &t : parts)
                e.tags << t.trimmed();
            e.favorite     = q.value(9).toBool();
            e.downloadable = q.value(10).toBool();
            e.notes        = q.value(11).toString();
            e.createdAt    = parseDateTime(q.value(12).toString());
            e.lastModified = parseDateTime(q.value(13).toString());
            loaded << e;
        }

        db.close();
    }

    QSqlDatabase::removeDatabase(connName);

    m_experiments = loaded;
    m_lastRefresh = QDateTime::currentDateTime();

    emit experimentsChanged(true, error);
    prefetchManifests();
}

// ---------------------------------------------------------------------------
// Manifests
// ---------------------------------------------------------------------------

void ExperimentService::prefetchManifests()
{
    for (const Experiment &e : m_experiments) {
        if (e.projectPath.isEmpty() || m_manifests.contains(e.projectPath))
            continue;
        ensureManifest(e.projectPath, [](const Manifest &) {});
    }
}

Manifest ExperimentService::loadLocalManifest(const QString &projectPath) const
{
    const QString path = experimentsRoot() + QLatin1Char('/') + projectPath
                       + QStringLiteral("/YLEDMETA.yaml");
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    return Manifest::parse(QString::fromUtf8(f.readAll()));
}

void ExperimentService::fetchRemoteManifest(const QString &projectPath, int candidate,
                                            std::function<void(const Manifest &)> done)
{
    if (candidate >= kManifestBaseUrls.size()) {
        done(Manifest{});
        return;
    }

    const QUrl url(kManifestBaseUrls.at(candidate) + projectPath
                   + QStringLiteral("/YLEDMETA.yaml"));
    QNetworkRequest req{url};

    QNetworkReply *reply = m_net.get(req);
    connect(reply, &QNetworkReply::finished, this,
            [this, reply, projectPath, candidate, done]() {
        const QByteArray data = reply->readAll();
        const bool ok = reply->error() == QNetworkReply::NoError && !data.trimmed().isEmpty();
        reply->deleteLater();

        if (ok)
            done(Manifest::parse(QString::fromUtf8(data)));
        else
            fetchRemoteManifest(projectPath, candidate + 1, done);
    });
}

void ExperimentService::ensureManifest(const QString &projectPath,
                                       std::function<void(const Manifest &)> done)
{
    auto it = m_manifests.constFind(projectPath);
    if (it != m_manifests.constEnd()) {
        done(it.value());
        return;
    }

    // A downloaded experiment carries its own manifest in its root.
    const Manifest local = loadLocalManifest(projectPath);
    if (local.valid) {
        m_manifests.insert(projectPath, local);
        done(local);
        return;
    }

    fetchRemoteManifest(projectPath, 0, [this, projectPath, done](const Manifest &m) {
        m_manifests.insert(projectPath, m);
        m_manifestAttempted.insert(projectPath);
        emit manifestChanged(projectPath);
        done(m);
    });
}

Manifest ExperimentService::manifestFor(const QString &projectPath) const
{
    const auto it = m_manifests.constFind(projectPath);
    if (it != m_manifests.constEnd() && it.value().valid)
        return it.value();

    const Manifest local = loadLocalManifest(projectPath);
    if (local.valid)
        return local;

    return it != m_manifests.constEnd() ? it.value() : Manifest{};
}

// ---------------------------------------------------------------------------
// Downloading
// ---------------------------------------------------------------------------

void ExperimentService::downloadToFile(const QString &url, const QString &filePath,
                                       const QString &projectPath, double percentMax,
                                       std::function<void(bool, const QString &)> done)
{
    QNetworkRequest req{QUrl(url)};
    QNetworkReply *reply = m_net.get(req);

    connect(reply, &QNetworkReply::downloadProgress, this,
            [this, projectPath, percentMax](qint64 received, qint64 total) {
        if (total > 0)
            emit downloadProgress(projectPath,
                                  static_cast<double>(received) / total * percentMax);
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, filePath, done]() {
        if (reply->error() != QNetworkReply::NoError) {
            const QString err = reply->errorString();
            reply->deleteLater();
            done(false, err);
            return;
        }

        const QByteArray data = reply->readAll();
        reply->deleteLater();

        QFile f(filePath);
        if (!f.open(QIODevice::WriteOnly)) {
            done(false, tr("Cannot write %1").arg(filePath));
            return;
        }
        f.write(data);
        f.close();
        done(true, QString());
    });
}

void ExperimentService::downloadExperiment(const QString &projectPath)
{
    if (projectPath.trimmed().isEmpty()) {
        emit downloadFinished(projectPath, false, tr("Project path is empty."));
        return;
    }

    if (m_downloading)
        return;

    m_downloading = true;
    emit downloadProgress(projectPath, 0);

    ensureManifest(projectPath, [this, projectPath](const Manifest &m) {
        const QString tgz = m.tgzUrl();
        const QString git = m.gitDirUrl();

        if (m.useTgz && !tgz.isEmpty())
            startTgzDownload(projectPath, tgz, m);
        else if (m.useGitDir && !git.isEmpty())
            startGitDirDownload(projectPath, git, m);
        else if (!git.isEmpty())
            startGitDirDownload(projectPath, git, m);
        else if (!tgz.isEmpty())
            startTgzDownload(projectPath, tgz, m);
        else
            startLegacyZipDownload(projectPath, m);
    });
}

void ExperimentService::startTgzDownload(const QString &projectPath,
                                         const QString &url, const Manifest &m)
{
    const QString temp = m_cacheDir + QLatin1Char('/') + projectPath + QStringLiteral(".tar.gz");

    downloadToFile(url, temp, projectPath, 90.0,
                   [this, projectPath, temp, m](bool ok, const QString &err) {
        if (!ok) {
            m_downloading = false;
            emit downloadFinished(projectPath, false,
                                  tr("Download failed: %1").arg(err));
            return;
        }

        emit downloadProgress(projectPath, 92);

        const QString targetDir = experimentsRoot() + QLatin1Char('/') + projectPath;
        removeDir(targetDir);
        QDir().mkpath(targetDir);

        QString extractError;
        extractArchive(temp, targetDir, &extractError);
        QFile::remove(temp);

        if (!extractError.isEmpty()) {
            m_downloading = false;
            emit downloadFinished(projectPath, false, extractError);
            return;
        }

        flattenNested(targetDir, projectPath);
        finalizeDownload(projectPath, m);
    });
}

void ExperimentService::startGitDirDownload(const QString &projectPath,
                                            const QString &url, const Manifest &m)
{
    QString owner, repo, branch, subPath;
    if (!parseGithubTreeUrl(url, &owner, &repo, &branch, &subPath)) {
        m_downloading = false;
        emit downloadFinished(projectPath, false,
                              tr("Unsupported download link: %1").arg(url));
        return;
    }

    const QString git = QStandardPaths::findExecutable(QStringLiteral("git"));
    if (git.isEmpty()) {
        startGitDirTarball(owner, repo, branch, subPath, projectPath, m);
        return;
    }

    emit downloadProgress(projectPath, 5);

    const QString repoUrl = QStringLiteral("https://github.com/%1/%2.git").arg(owner, repo);
    const QString cloneDir = m_cacheDir + QStringLiteral("/git-") + projectPath
                           + QLatin1Char('-')
                           + QString::number(QDateTime::currentMSecsSinceEpoch());
    removeDir(cloneDir);

    auto *clone = new QProcess(this);
    clone->setProgram(git);
    clone->setArguments({ QStringLiteral("clone"), QStringLiteral("--depth"),
                          QStringLiteral("1"), QStringLiteral("--filter=blob:none"),
                          QStringLiteral("--sparse"), repoUrl, cloneDir });
    clone->setWorkingDirectory(m_cacheDir);

    connect(clone, &QProcess::finished, this,
            [this, clone, git, owner, repo, branch, subPath, projectPath, m, cloneDir]
            (int code, QProcess::ExitStatus status) {
        clone->deleteLater();

        if (code != 0 || status != QProcess::NormalExit) {
            removeDir(cloneDir);
            startGitDirTarball(owner, repo, branch, subPath, projectPath, m);
            return;
        }

        emit downloadProgress(projectPath, 55);

        // A tree URL pointing at the repo root has no sub-path to check out.
        if (subPath.isEmpty()) {
            const QString targetDir = experimentsRoot() + QLatin1Char('/') + projectPath;
            removeDir(targetDir);
            QDir().mkpath(targetDir);
            copyDirRecursive(cloneDir, targetDir);
            removeDir(cloneDir);
            finalizeDownload(projectPath, m);
            return;
        }

        auto *sparse = new QProcess(this);
        sparse->setProgram(git);
        sparse->setArguments({ QStringLiteral("-C"), cloneDir,
                              QStringLiteral("sparse-checkout"),
                              QStringLiteral("set"), subPath });
        sparse->setWorkingDirectory(m_cacheDir);

        connect(sparse, &QProcess::finished, this,
                [this, sparse, owner, repo, branch, subPath, projectPath, m, cloneDir]
                (int code2, QProcess::ExitStatus status2) {
            sparse->deleteLater();

            if (code2 != 0 || status2 != QProcess::NormalExit) {
                removeDir(cloneDir);
                startGitDirTarball(owner, repo, branch, subPath, projectPath, m);
                return;
            }

            const QString srcDir = cloneDir + QLatin1Char('/') + subPath;
            const QString targetDir = experimentsRoot() + QLatin1Char('/') + projectPath;

            removeDir(targetDir);
            QDir().mkpath(targetDir);
            copyDirRecursive(srcDir, targetDir);
            removeDir(cloneDir);

            if (QDir(targetDir).entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty()) {
                m_downloading = false;
                emit downloadFinished(projectPath, false,
                                      tr("Downloaded folder was empty."));
                return;
            }

            finalizeDownload(projectPath, m);
        });

        sparse->start();
    });

    clone->start();
}

void ExperimentService::startGitDirTarball(const QString &owner, const QString &repo,
                                           const QString &branch, const QString &subPath,
                                           const QString &projectPath, const Manifest &m)
{
    emit downloadProgress(projectPath, 10);

    const QString url = QStringLiteral("https://codeload.github.com/%1/%2/tar.gz/refs/heads/%3")
                            .arg(owner, repo, branch);
    const QString temp = m_cacheDir + QLatin1Char('/') + projectPath + QStringLiteral(".git.tar.gz");
    const QString extractDir = m_cacheDir + QStringLiteral("/git-") + projectPath + QStringLiteral("-x");

    downloadToFile(url, temp, projectPath, 85.0,
                   [this, projectPath, temp, extractDir, subPath, m](bool ok, const QString &err) {
        if (!ok) {
            QFile::remove(temp);
            m_downloading = false;
            emit downloadFinished(projectPath, false,
                                  tr("Download failed: %1").arg(err));
            return;
        }

        emit downloadProgress(projectPath, 90);

        removeDir(extractDir);
        QDir().mkpath(extractDir);

        QString extractError;
        extractArchive(temp, extractDir, &extractError);
        QFile::remove(temp);

        if (!extractError.isEmpty()) {
            removeDir(extractDir);
            m_downloading = false;
            emit downloadFinished(projectPath, false, extractError);
            return;
        }

        const QDir top(extractDir);
        const QStringList dirs = top.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        QString src = extractDir;
        if (dirs.size() == 1)
            src = top.filePath(dirs.first());
        if (!subPath.isEmpty())
            src += QLatin1Char('/') + subPath;

        const QString targetDir = experimentsRoot() + QLatin1Char('/') + projectPath;
        removeDir(targetDir);
        QDir().mkpath(targetDir);
        copyDirRecursive(src, targetDir);
        removeDir(extractDir);

        if (QDir(targetDir).entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty()) {
            m_downloading = false;
            emit downloadFinished(projectPath, false,
                                  tr("Downloaded folder was empty."));
            return;
        }

        finalizeDownload(projectPath, m);
    });
}

void ExperimentService::startLegacyZipDownload(const QString &projectPath, const Manifest &m)
{
    const QString zipUrl = kStorageBaseUrl
        + QStringLiteral("Exp/Bin/win/") + projectPath + QStringLiteral(".zip");
    const QString tempZip = m_cacheDir + QLatin1Char('/') + projectPath + QStringLiteral(".zip");

    downloadToFile(zipUrl, tempZip, projectPath, 90.0,
                   [this, projectPath, tempZip, m](bool ok, const QString &err) {
        if (!ok) {
            m_downloading = false;
            emit downloadFinished(projectPath, false,
                                  tr("Download failed: %1").arg(err));
            return;
        }

        emit downloadProgress(projectPath, 92);

        const QString targetDir = experimentsRoot() + QLatin1Char('/') + projectPath;
        removeDir(targetDir);
        QDir().mkpath(targetDir);

        QString extractError;
        const bool extracted = extractZip(tempZip, targetDir, &extractError);
        QFile::remove(tempZip);

        if (!extracted) {
            m_downloading = false;
            emit downloadFinished(projectPath, false, extractError);
            return;
        }

        flattenNested(targetDir, projectPath);
        finalizeDownload(projectPath, m);
    });
}

void ExperimentService::finalizeDownload(const QString &projectPath, const Manifest &m)
{
    const QString targetDir = experimentsRoot() + QLatin1Char('/') + projectPath;
    QDir().mkpath(targetDir);

    // Every experiment root gets a YLEDMETA.yaml.
    const QString metaPath = targetDir + QStringLiteral("/YLEDMETA.yaml");
    if (!QFile::exists(metaPath)) {
        QString text = m.raw;
        if (text.trimmed().isEmpty()) {
            text = QStringLiteral("# YLEDMETA.yaml generated by YLED.\n"
                                  "# See the YLED README for the full schema.\n"
                                  "run:\n"
                                  "  windows: \"run.bat\"\n"
                                  "  linux: \"./run.sh\"\n");
        }
        QFile f(metaPath);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            f.write(text.toUtf8());
            f.close();
        }
    }

    // Cache the effective manifest (prefer the one now on disk).
    const Manifest local = loadLocalManifest(projectPath);
    m_manifests.insert(projectPath, local.valid ? local : m);
    const Manifest eff = local.valid ? local : m;

    // Run the manifest `init` command (best effort).
    const QString init = eff.initCommand();
    if (!init.isEmpty())
        runShellBlocking(init, targetDir);

    QFile marker(targetDir + QStringLiteral("/.yled.ok"));
    if (marker.open(QIODevice::WriteOnly))
        marker.close();

    emit downloadProgress(projectPath, 100);
    m_downloading = false;
    emit manifestChanged(projectPath);
    emit downloadFinished(projectPath, true, QString());
}

void ExperimentService::extractArchive(const QString &archivePath, const QString &destDir,
                                       QString *error) const
{
    if (error)
        error->clear();

    int rc = QProcess::execute(QStringLiteral("tar"),
                               { QStringLiteral("-xf"), archivePath,
                                 QStringLiteral("-C"), destDir });
    if (rc != 0) {
        rc = QProcess::execute(QStringLiteral("7z"),
                               { QStringLiteral("x"), archivePath,
                                 QStringLiteral("-o") + destDir, QStringLiteral("-y") });
    }

    if (rc != 0 && error)
        *error = tr("Failed to extract %1").arg(QFileInfo(archivePath).fileName());
}

bool ExperimentService::extractZip(const QString &zipPath, const QString &destDir, QString *error)
{
    int rc = -1;

#ifdef Q_OS_WIN
    const QStringList ps = {
        QStringLiteral("-NoProfile"),
        QStringLiteral("-Command"),
        QStringLiteral("Expand-Archive -LiteralPath '%1' -DestinationPath '%2' -Force")
            .arg(zipPath, destDir)
    };
    rc = QProcess::execute(QStringLiteral("powershell"), ps);
    if (rc != 0)
        rc = QProcess::execute(QStringLiteral("powershell.exe"), ps);
#else
    rc = QProcess::execute(QStringLiteral("unzip"),
                           { QStringLiteral("-o"), zipPath, QStringLiteral("-d"), destDir });
    if (rc != 0)
        rc = QProcess::execute(QStringLiteral("python3"),
                               { QStringLiteral("-m"), QStringLiteral("zipfile"),
                                 QStringLiteral("-e"), zipPath, destDir });
#endif

    if (rc != 0) {
        if (error)
            *error = tr("Failed to extract %1").arg(QFileInfo(zipPath).fileName());
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Running
// ---------------------------------------------------------------------------

bool ExperimentService::isDownloaded(const QString &projectPath) const
{
    if (projectPath.trimmed().isEmpty())
        return false;

    const QString dir = experimentsRoot() + QLatin1Char('/') + projectPath;
    if (QFile::exists(dir + QStringLiteral("/.yled.ok")))
        return true;

    return QFile::exists(dir + QStringLiteral("/run.sh"))
        || QFile::exists(dir + QStringLiteral("/run.bat"));
}

void ExperimentService::runExperiment(const QString &projectPath, QString *error)
{
    if (error)
        error->clear();

    const QString dir = experimentsRoot() + QLatin1Char('/') + projectPath;
    const Manifest m = manifestFor(projectPath);
    const QString platform = Manifest::platform();

    QString cmd = m.valid ? m.runCommand() : QString();
    if (cmd.isEmpty())
        cmd = (platform == QLatin1String("windows")) ? QStringLiteral("run.bat")
                                                     : QStringLiteral("./run.sh");

#ifdef Q_OS_WIN
    if (cmd.startsWith(QLatin1String("./")))
        cmd = cmd.mid(2);

    if (!QProcess::startDetached(QStringLiteral("cmd"),
                                 { QStringLiteral("/c"), cmd }, dir)) {
        if (error)
            *error = tr("Failed to launch: %1").arg(cmd);
    }
    Q_UNUSED(dir);
#else
    // Windows-only package running on Linux/macOS: use Wine when available.
    if (!QFile::exists(dir + QStringLiteral("/run.sh")) &&
        QFile::exists(dir + QStringLiteral("/run.bat"))) {
        const QString wine = QStandardPaths::findExecutable(QStringLiteral("wine"));
        if (!wine.isEmpty()) {
            if (!QProcess::startDetached(wine,
                                         { QStringLiteral("cmd"), QStringLiteral("/c"),
                                           QStringLiteral("run.bat") }, dir)) {
                if (error)
                    *error = tr("Failed to launch run.bat through Wine.");
            }
            return;
        }
    }

    // Make a referenced shell script executable before running it.
    const QString script = extractScriptName(cmd);
    if (!script.isEmpty()) {
        const QString path = dir + QLatin1Char('/') + script;
        if (QFile::exists(path)) {
            QFile f(path);
            f.setPermissions(f.permissions()
                             | QFileDevice::ExeOwner
                             | QFileDevice::ExeGroup
                             | QFileDevice::ExeOther);
        }
    }

    if (!QProcess::startDetached(QStringLiteral("bash"),
                                 { QStringLiteral("-c"), cmd }, dir)) {
        if (error)
            *error = tr("Failed to launch: %1").arg(cmd);
    }
#endif
}

void ExperimentService::clearCache()
{
    QFile::remove(m_dbPath);
}

Experiment ExperimentService::experimentById(int id) const
{
    for (const Experiment &e : m_experiments)
        if (e.id == id)
            return e;
    return {};
}
