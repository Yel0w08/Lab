#include "core/ExperimentService.h"

#include "core/DatabaseConfig.h"

#include <QDate>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

namespace {

QSqlDatabase db()
{
    return QSqlDatabase::database(QStringLiteral("labmanager"));
}

/** EF Core stores DateTime as `yyyy-MM-dd HH:mm:ss.fffffff`. */
QString toDbDateTime(const QDateTime &dt)
{
    if (!dt.isValid())
        return QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    return dt.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
}

QDateTime fromDbDateTime(const QString &value)
{
    QString text = value.trimmed();
    if (text.isEmpty())
        return {};

    // Keep only milliseconds, whatever the precision the row was written with.
    const int dot = text.indexOf(QLatin1Char('.'), 19);
    if (dot >= 0 && text.size() > dot + 4)
        text = text.left(dot + 4);

    QDateTime dt = QDateTime::fromString(text, QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    if (!dt.isValid())
        dt = QDateTime::fromString(text, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (!dt.isValid())
        dt = QDateTime::fromString(text, Qt::ISODate);
    return dt;
}

Experiment recordToExperiment(const QSqlQuery &query)
{
    const QSqlRecord record = query.record();
    const auto field = [&record, &query](const char *name) {
        return query.value(record.indexOf(QLatin1String(name)));
    };

    Experiment e;
    e.id          = field("Id").toInt();
    e.name        = field("Name").toString();
    e.description = field("Description").toString();
    e.language    = field("Language").toString();
    e.framework   = field("Framework").toString();
    e.engine      = field("Engine").toString();
    e.status      = Status::fromKey(field("Status").toString());
    e.projectPath = field("ProjectPath").toString();

    e.tags = splitTags(field("Tags").toString());

    e.favorite = field("Favorite").toInt() != 0;

    const QVariant downloadable = field("Downloadable");
    e.downloadable = !downloadable.isNull() && downloadable.toInt() != 0;

    e.notes        = field("Notes").toString();
    e.createdAt    = fromDbDateTime(field("CreatedAt").toString());
    e.lastModified = fromDbDateTime(field("LastModified").toString());
    return e;
}

bool bindExperiment(QSqlQuery &query, const Experiment &e, bool withId)
{
    if (withId)
        query.addBindValue(e.id);

    query.addBindValue(e.name);
    query.addBindValue(e.description);
    query.addBindValue(e.language);
    query.addBindValue(e.framework);
    query.addBindValue(e.engine);
    query.addBindValue(Status::key(e.status));
    query.addBindValue(e.projectPath);
    query.addBindValue(e.tags.join(QLatin1Char(',')));
    query.addBindValue(e.favorite ? 1 : 0);
    query.addBindValue(e.notes);
    query.addBindValue(toDbDateTime(e.createdAt));
    query.addBindValue(toDbDateTime(e.lastModified));
    query.addBindValue(e.downloadable ? 1 : 0);
    return true;
}

const char *kInsertSql =
    "INSERT INTO \"Experiments\" "
    "(\"Name\", \"Description\", \"Language\", \"Framework\", \"Engine\", \"Status\", "
    "\"ProjectPath\", \"Tags\", \"Favorite\", \"Notes\", \"CreatedAt\", \"LastModified\", \"Downloadable\") "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

} // namespace

ExperimentService::ExperimentService()
{
    if (!DatabaseConfig::isConnected()) {
        QString error;
        if (!DatabaseConfig::openConnection(&error))
            m_error = error;
    }
}

bool ExperimentService::isValid() const
{
    return DatabaseConfig::isConnected();
}

QString ExperimentService::lastError() const
{
    return m_error.isEmpty() ? DatabaseConfig::lastError() : m_error;
}

QList<Experiment> ExperimentService::getAll() const
{
    QList<Experiment> result;
    if (!isValid())
        return result;

    QSqlQuery query(db());
    if (!query.exec(QStringLiteral(
            "SELECT * FROM \"Experiments\" ORDER BY \"Favorite\" DESC, \"LastModified\" DESC"))) {
        m_error = query.lastError().text();
        return result;
    }

    while (query.next())
        result.append(recordToExperiment(query));
    return result;
}

std::optional<Experiment> ExperimentService::getById(int id) const
{
    if (!isValid())
        return std::nullopt;

    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT * FROM \"Experiments\" WHERE \"Id\" = ?"));
    query.addBindValue(id);
    if (!query.exec()) {
        m_error = query.lastError().text();
        return std::nullopt;
    }
    if (!query.next())
        return std::nullopt;
    return recordToExperiment(query);
}

QList<Experiment> ExperimentService::search(const QString &text) const
{
    const QString needle = text.trimmed();
    if (needle.isEmpty())
        return getAll();

    const auto all = getAll();
    QList<Experiment> result;
    for (const Experiment &e : all) {
        if (e.name.contains(needle, Qt::CaseInsensitive)
            || e.description.contains(needle, Qt::CaseInsensitive)
            || e.language.contains(needle, Qt::CaseInsensitive)
            || e.framework.contains(needle, Qt::CaseInsensitive)
            || e.engine.contains(needle, Qt::CaseInsensitive)
            || e.notes.contains(needle, Qt::CaseInsensitive)
            || e.projectPath.contains(needle, Qt::CaseInsensitive)) {
            result.append(e);
            continue;
        }
        for (const QString &tag : e.tags) {
            if (tag.contains(needle, Qt::CaseInsensitive)) {
                result.append(e);
                break;
            }
        }
    }
    return result;
}

QList<Experiment> ExperimentService::getByStatus(ExperimentStatus status) const
{
    const auto all = getAll();
    QList<Experiment> result;
    for (const Experiment &e : all) {
        if (e.status == status)
            result.append(e);
    }
    return result;
}

bool ExperimentService::add(Experiment &experiment)
{
    if (!isValid())
        return false;

    if (experiment.createdAt.isNull())
        experiment.createdAt = QDateTime::currentDateTime();
    experiment.lastModified = QDateTime::currentDateTime();

    QSqlQuery query(db());
    query.prepare(QLatin1String(kInsertSql));
    bindExperiment(query, experiment, false);

    if (!query.exec()) {
        m_error = query.lastError().text();
        return false;
    }

    experiment.id = query.lastInsertId().toInt();
    return true;
}

bool ExperimentService::update(const Experiment &experiment)
{
    if (!isValid())
        return false;

    Experiment stored = experiment;
    stored.lastModified = QDateTime::currentDateTime();

    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE \"Experiments\" SET "
        "\"Name\" = ?, \"Description\" = ?, \"Language\" = ?, \"Framework\" = ?, \"Engine\" = ?, "
        "\"Status\" = ?, \"ProjectPath\" = ?, \"Tags\" = ?, \"Favorite\" = ?, \"Notes\" = ?, "
        "\"CreatedAt\" = ?, \"LastModified\" = ?, \"Downloadable\" = ? "
        "WHERE \"Id\" = ?"));
    bindExperiment(query, stored, false);
    query.addBindValue(stored.id);

    if (!query.exec()) {
        m_error = query.lastError().text();
        return false;
    }
    return true;
}

bool ExperimentService::remove(int id)
{
    if (!isValid())
        return false;

    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM \"Experiments\" WHERE \"Id\" = ?"));
    query.addBindValue(id);
    if (!query.exec()) {
        m_error = query.lastError().text();
        return false;
    }
    return true;
}

int ExperimentService::count() const
{
    if (!isValid())
        return 0;

    QSqlQuery query(db());
    if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM \"Experiments\"")) || !query.next())
        return 0;
    return query.value(0).toInt();
}

bool ExperimentService::importFromJsonIfEmpty(const QString &jsonPath, QString *error)
{
    QFile file(jsonPath);
    if (!file.exists())
        return true;

    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = file.errorString();
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (doc.isNull()) {
        if (error)
            *error = parseError.errorString();
        return false;
    }

    QJsonArray experiments;
    const QJsonObject root = doc.object();
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key().compare(QLatin1String("experiments"), Qt::CaseInsensitive) == 0
            && it.value().isArray()) {
            experiments = it.value().toArray();
            break;
        }
    }

    if (experiments.isEmpty())
        return true;

    ExperimentService service;
    if (!service.isValid()) {
        if (error)
            *error = service.lastError();
        return false;
    }
    if (service.count() > 0)
        return true;

    for (const QJsonValue &value : experiments) {
        const QJsonObject obj = value.toObject();
        if (obj.isEmpty())
            continue;

        Experiment e;
        e.id          = obj.value(QStringLiteral("Id")).toInt(0);
        e.name        = obj.value(QStringLiteral("Name")).toString();
        e.description = obj.value(QStringLiteral("Description")).toString();
        e.language    = obj.value(QStringLiteral("Language")).toString();
        e.framework   = obj.value(QStringLiteral("Framework")).toString();
        e.engine      = obj.value(QStringLiteral("Engine")).toString();
        e.projectPath = obj.value(QStringLiteral("ProjectPath")).toString();
        e.notes       = obj.value(QStringLiteral("Notes")).toString();
        e.favorite    = obj.value(QStringLiteral("Favorite")).toBool(false);
        e.downloadable = obj.value(QStringLiteral("Downloadable")).toBool(false);

        const QJsonValue status = obj.value(QStringLiteral("Status"));
        e.status = status.isString()
                       ? Status::fromKey(status.toString())
                       : Status::fromKey(QString::number(status.toInt(0)));

        for (const QJsonValue &tag : obj.value(QStringLiteral("Tags")).toArray())
            e.tags << tag.toString();

        e.createdAt = fromDbDateTime(obj.value(QStringLiteral("CreatedAt")).toString());
        if (!e.createdAt.isValid())
            e.createdAt = QDateTime::currentDateTime();
        e.lastModified = fromDbDateTime(obj.value(QStringLiteral("LastModified")).toString());
        if (!e.lastModified.isValid())
            e.lastModified = e.createdAt;

        if (e.name.trimmed().isEmpty())
            continue;

        if (!service.add(e) && error)
            *error = service.lastError();
    }

    return true;
}
