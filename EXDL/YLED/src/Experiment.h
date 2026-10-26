#pragma once

#include <QString>
#include <QStringList>
#include <QDateTime>

// Mirrors the Experiment row of the LabManager.db table.
struct Experiment
{
    int         id = 0;
    QString     name;
    QString     description;
    QString     language;
    QString     framework;
    QString     engine;
    QString     status;
    QString     projectPath;
    QStringList tags;
    bool        favorite = false;
    bool        downloadable = false;
    QString     notes;
    QDateTime   createdAt;
    QDateTime   lastModified;
};