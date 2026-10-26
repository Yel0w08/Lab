#pragma once

#include <QWidget>
#include "Experiment.h"

class QLabel;
class QPushButton;
class QProgressBar;
class QVBoxLayout;
class ExperimentService;

// Experiment detail page: full info, download with progress, and run.
class PageExperimentDetail : public QWidget
{
    Q_OBJECT
public:
    explicit PageExperimentDetail(ExperimentService *service, QWidget *parent = nullptr);

    void showExperiment(const Experiment &e);

signals:
    void back();

private:
    void render();
    void startDownload();
    void run();

    ExperimentService *m_service;
    Experiment m_exp;
    bool m_hasExp = false;
    bool m_downloaded = false;
    bool m_downloading = false;
    double m_progress = 0.0;
    QString m_status;

    QVBoxLayout *m_body;
    QLabel *m_statusLabel;
    QLabel *m_manifestLabel;
    QWidget *m_contentHost;
};