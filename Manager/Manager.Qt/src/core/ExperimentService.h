#pragma once

#include "core/Experiment.h"

#include <QList>
#include <QString>
#include <optional>

/**
 * Thin data access layer over the shared SQLite database.
 * Behaviour matches the WinForms / Avalonia `ExperimentService`.
 */
class ExperimentService
{
public:
    ExperimentService();

    bool isValid() const;
    QString lastError() const;

    /** All experiments: favourites first, then most recently modified. */
    QList<Experiment> getAll() const;
    std::optional<Experiment> getById(int id) const;
    QList<Experiment> search(const QString &text) const;
    QList<Experiment> getByStatus(ExperimentStatus status) const;

    /** Inserts the row and stores the generated id into the given experiment. */
    bool add(Experiment &experiment);
    bool update(const Experiment &experiment);
    bool remove(int id);

    int count() const;

    /**
     * Imports the legacy `ExperimentsDB.json` export, but only when the
     * database is still empty (so nothing ever gets duplicated).
     */
    static bool importFromJsonIfEmpty(const QString &jsonPath, QString *error = nullptr);

private:
    mutable QString m_error;
};
