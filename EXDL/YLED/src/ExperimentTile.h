#pragma once

#include <QFrame>
#include "Experiment.h"

class QToolButton;
class QProgressBar;
class QLabel;
class ExperimentService;

// A single experiment card shown in the Experiments list, with a small
// hand-drawn download icon button (no emoji, no external assets) that downloads
// the package straight from the tile.
class ExperimentTile : public QFrame
{
    Q_OBJECT
public:
    ExperimentTile(const Experiment &e, ExperimentService *service,
                   QWidget *parent = nullptr);

signals:
    void openDetails(int id);

private:
    void refresh();
    void onIconClicked();
    void startDownload();
    void run();

    Experiment m_exp;
    ExperimentService *m_service;
    QToolButton *m_iconBtn = nullptr;
    QProgressBar *m_progress = nullptr;
    QLabel *m_manifestLabel = nullptr;
    bool m_downloading = false;
    bool m_downloaded = false;
    double m_percent = 0.0;
    QString m_error;
};