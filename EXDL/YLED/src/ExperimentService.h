#pragma once

#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>
#include <QString>
#include <QDateTime>
#include <QNetworkAccessManager>

#include <functional>

#include "Experiment.h"
#include "Manifest.h"

// Downloads the experiment database (SQLite) from GitHub, reads the experiment
// list, and manages downloading / extracting / launching experiments using each
// experiment's YLEDMETA.yaml manifest.
class ExperimentService : public QObject
{
    Q_OBJECT
public:
    explicit ExperimentService(QObject *parent = nullptr);

    // Fetch (or serve from cache) the experiment list.
    // force == true bypasses the 5-minute in-memory cache and re-reads the DB.
    void requestExperiments(bool force = false);

    // Manifest-driven download of an experiment into the local experiments
    // folder, followed by the manifest `init` command.
    void downloadExperiment(const QString &projectPath);

    bool isDownloaded(const QString &projectPath) const;
    bool isDownloading() const { return m_downloading; }

    // Launch the experiment using its YLEDMETA.yaml `run` command. Falls back to
    // run.bat / run.sh, then to Wine for Windows-only packages on Unix.
    void runExperiment(const QString &projectPath, QString *error = nullptr);

    void clearCache();

    const QList<Experiment> &experiments() const { return m_experiments; }
    Experiment experimentById(int id) const;
    QString experimentsRoot() const;

    // Cached remote manifest, the local experiment's YLEDMETA.yaml, or an
    // invalid Manifest when neither exists.
    Manifest manifestFor(const QString &projectPath) const;
    static QString platformName() { return Manifest::platform(); }

signals:
    void experimentsChanged(bool ok, const QString &error);
    void manifestChanged(const QString &projectPath);
    void downloadProgress(const QString &projectPath, double percent);
    void downloadFinished(const QString &projectPath, bool ok, const QString &error);

private:
    void downloadDatabase();
    void loadFromDatabase();

    void prefetchManifests();
    void fetchRemoteManifest(const QString &projectPath, int candidate,
                             std::function<void(const Manifest &)> done);
    Manifest loadLocalManifest(const QString &projectPath) const;
    void ensureManifest(const QString &projectPath,
                        std::function<void(const Manifest &)> done);

    void downloadToFile(const QString &url, const QString &filePath,
                        const QString &projectPath, double percentMax,
                        std::function<void(bool, const QString &)> done);

    void startTgzDownload(const QString &projectPath, const QString &url, const Manifest &m);
    void startGitDirDownload(const QString &projectPath, const QString &url, const Manifest &m);
    void startGitDirTarball(const QString &owner, const QString &repo, const QString &branch,
                            const QString &subPath, const QString &projectPath, const Manifest &m);
    void startLegacyZipDownload(const QString &projectPath, const Manifest &m);
    void finalizeDownload(const QString &projectPath, const Manifest &m);

    void extractArchive(const QString &archivePath, const QString &destDir,
                        QString *error) const;

    static bool extractZip(const QString &zipPath, const QString &destDir, QString *error);

    QNetworkAccessManager m_net;
    QList<Experiment> m_experiments;
    QHash<QString, Manifest> m_manifests;
    QSet<QString> m_manifestAttempted;
    QString m_cacheDir;
    QString m_appDataDir;
    QString m_dbPath;
    QDateTime m_lastRefresh;
    bool m_dbPending = false;
    bool m_downloading = false;
};
