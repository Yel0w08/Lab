#pragma once

#include <QColor>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>

/**
 * The status of an experiment.
 * The `key()` spelling is what gets stored in the database so the Qt, WinForms
 * and Avalonia managers can all share the very same LabManager.db file.
 */
enum class ExperimentStatus {
    Planned = 0,
    InProgress,
    Finished,
    Archived,
    Abandoned
};

namespace Status {

inline QList<ExperimentStatus> all()
{
    return {
        ExperimentStatus::Planned,
        ExperimentStatus::InProgress,
        ExperimentStatus::Finished,
        ExperimentStatus::Archived,
        ExperimentStatus::Abandoned
    };
}

inline QString key(ExperimentStatus status)
{
    switch (status) {
    case ExperimentStatus::Planned:    return QStringLiteral("Planned");
    case ExperimentStatus::InProgress: return QStringLiteral("InProgress");
    case ExperimentStatus::Finished:   return QStringLiteral("Finished");
    case ExperimentStatus::Archived:   return QStringLiteral("Archived");
    case ExperimentStatus::Abandoned:  return QStringLiteral("Abandoned");
    }
    return QStringLiteral("Planned");
}

inline QString label(ExperimentStatus status)
{
    switch (status) {
    case ExperimentStatus::Planned:    return QStringLiteral("Planned");
    case ExperimentStatus::InProgress: return QStringLiteral("In Progress");
    case ExperimentStatus::Finished:   return QStringLiteral("Finished");
    case ExperimentStatus::Archived:   return QStringLiteral("Archived");
    case ExperimentStatus::Abandoned:  return QStringLiteral("Abandoned");
    }
    return QStringLiteral("Unknown");
}

inline QString description(ExperimentStatus status)
{
    switch (status) {
    case ExperimentStatus::Planned:    return QStringLiteral("Idea queued, not started yet");
    case ExperimentStatus::InProgress: return QStringLiteral("Currently being built");
    case ExperimentStatus::Finished:   return QStringLiteral("Done and working");
    case ExperimentStatus::Archived:   return QStringLiteral("Parked, kept for reference");
    case ExperimentStatus::Abandoned:  return QStringLiteral("Stopped, not worth finishing");
    }
    return {};
}

inline ExperimentStatus fromKey(const QString &key)
{
    const QString compact = key.trimmed().toLower().remove(QLatin1Char(' '));

    if (compact == QLatin1String("inprogress")) return ExperimentStatus::InProgress;
    if (compact == QLatin1String("finished"))   return ExperimentStatus::Finished;
    if (compact == QLatin1String("archived"))   return ExperimentStatus::Archived;
    if (compact == QLatin1String("abandoned"))  return ExperimentStatus::Abandoned;
    if (compact == QLatin1String("planned"))    return ExperimentStatus::Planned;

    // Enum serialized as a number (the legacy JSON export).
    bool ok = false;
    const int value = compact.toInt(&ok);
    if (ok && value >= 0 && value <= 4)
        return static_cast<ExperimentStatus>(value);

    return ExperimentStatus::Planned;
}

inline QColor color(ExperimentStatus status)
{
    switch (status) {
    case ExperimentStatus::Planned:    return QColor(QStringLiteral("#8EA3C2"));
    case ExperimentStatus::InProgress: return QColor(QStringLiteral("#FFB648"));
    case ExperimentStatus::Finished:   return QColor(QStringLiteral("#3DDC97"));
    case ExperimentStatus::Archived:   return QColor(QStringLiteral("#A78BFA"));
    case ExperimentStatus::Abandoned:  return QColor(QStringLiteral("#FF5D73"));
    }
    return QColor(QStringLiteral("#8EA3C2"));
}

} // namespace Status

/** Splits a comma separated tag list, trimming every entry. */
inline QStringList splitTags(const QString &text)
{
    QStringList tags;
    const QStringList parts = text.split(QLatin1Char(','), Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        const QString tag = part.trimmed();
        if (!tag.isEmpty())
            tags << tag;
    }
    return tags;
}

struct Experiment {
    int id = 0;

    QString name;
    QString description;

    QString language;
    QString framework;
    QString engine;

    ExperimentStatus status = ExperimentStatus::Planned;

    QString projectPath;

    QStringList tags;
    bool favorite = false;
    bool downloadable = false;

    QString notes;

    QDateTime createdAt;
    QDateTime lastModified;

    QString tagsDisplay() const { return tags.join(QStringLiteral(", ")); }

    QString techDisplay() const
    {
        QStringList parts;
        if (!language.isEmpty())  parts << language;
        if (!framework.isEmpty()) parts << framework;
        if (!engine.isEmpty())    parts << engine;
        return parts.join(QStringLiteral("  ·  "));
    }

    bool hasProjectPath() const { return !projectPath.trimmed().isEmpty(); }
};
