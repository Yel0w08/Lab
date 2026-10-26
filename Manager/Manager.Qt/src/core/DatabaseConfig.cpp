#include "core/DatabaseConfig.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace DatabaseConfig {
namespace {

const char *kConnectionName = "labmanager";
QString g_lastError;

QString findRootFrom(QString start)
{
    if (start.isEmpty())
        return {};

    QDir dir(QDir::cleanPath(start));
    while (true) {
        if (QFileInfo::exists(dir.filePath(QStringLiteral(".git"))))
            return dir.absolutePath();
        if (!dir.cdUp())
            break;
    }
    return {};
}

bool ensureSchema(QSqlDatabase &db, QString *error)
{
    QSqlQuery query(db);

    const char *createSql =
        "CREATE TABLE IF NOT EXISTS \"Experiments\" ("
        " \"Id\" INTEGER NOT NULL CONSTRAINT \"PK_Experiments\" PRIMARY KEY AUTOINCREMENT,"
        " \"Name\" TEXT NOT NULL,"
        " \"Description\" TEXT NOT NULL,"
        " \"Language\" TEXT NOT NULL,"
        " \"Framework\" TEXT NOT NULL,"
        " \"Engine\" TEXT NOT NULL,"
        " \"Status\" TEXT NOT NULL,"
        " \"ProjectPath\" TEXT NOT NULL,"
        " \"Tags\" TEXT NOT NULL,"
        " \"Favorite\" INTEGER NOT NULL,"
        " \"Notes\" TEXT NOT NULL,"
        " \"CreatedAt\" TEXT NOT NULL,"
        " \"LastModified\" TEXT NOT NULL,"
        " \"Downloadable\" BOOLEAN)";

    if (!query.exec(QLatin1String(createSql))) {
        if (error)
            *error = query.lastError().text();
        return false;
    }

    // Older databases may miss the Downloadable column added later on.
    bool hasDownloadable = false;
    if (query.exec(QStringLiteral("PRAGMA table_info(\"Experiments\")"))) {
        while (query.next()) {
            if (query.value(1).toString().compare(QLatin1String("Downloadable"),
                                                  Qt::CaseInsensitive) == 0) {
                hasDownloadable = true;
                break;
            }
        }
    }

    if (!hasDownloadable
        && !query.exec(QStringLiteral("ALTER TABLE \"Experiments\" ADD COLUMN \"Downloadable\" BOOLEAN"))) {
        if (error)
            *error = query.lastError().text();
        return false;
    }

    return true;
}

} // namespace

QString repositoryRoot()
{
    static QString root;
    static bool resolved = false;
    if (resolved)
        return root;

    resolved = true;

    const QStringList starts = {
        QDir::current().absolutePath(),
        QCoreApplication::applicationDirPath(),
#ifdef LAB_SOURCE_DIR
        QStringLiteral(LAB_SOURCE_DIR),
#endif
    };

    for (const QString &start : starts) {
        const QString found = findRootFrom(start);
        if (!found.isEmpty()) {
            root = found;
            break;
        }
    }

    if (root.isEmpty())
        root = QCoreApplication::applicationDirPath();

    return root;
}

QString databasePath()
{
    return QDir(repositoryRoot()).filePath(QStringLiteral("LabManager.db"));
}

bool openConnection(QString *error)
{
    QSqlDatabase db = QSqlDatabase::contains(QLatin1String(kConnectionName))
                          ? QSqlDatabase::database(QLatin1String(kConnectionName))
                          : QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
                                                      QLatin1String(kConnectionName));

    db.setDatabaseName(databasePath());

    if (!db.open()) {
        g_lastError = db.lastError().text();
        if (error)
            *error = g_lastError;
        return false;
    }

    QSqlQuery pragma(db);
    pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"));

    QString schemaError;
    if (!ensureSchema(db, &schemaError)) {
        g_lastError = schemaError;
        if (error)
            *error = schemaError;
        return false;
    }

    g_lastError.clear();
    return true;
}

bool isConnected()
{
    if (!QSqlDatabase::contains(QLatin1String(kConnectionName)))
        return false;
    return QSqlDatabase::database(QLatin1String(kConnectionName)).isOpen();
}

QString lastError()
{
    return g_lastError;
}

} // namespace DatabaseConfig
