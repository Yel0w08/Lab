#pragma once

#include <QWidget>

class QLabel;
class ExperimentService;

// Settings page: cache management and about info.
class PageSettings : public QWidget
{
    Q_OBJECT
public:
    explicit PageSettings(ExperimentService *service, QWidget *parent = nullptr);

private:
    void clearCache();
    void forceRefresh();
    void refreshManifests();
    void setStatus(const QString &text, const char *kind);

    ExperimentService *m_service;
    QLabel *m_statusLabel;
    bool m_waitingForRefresh = false;
};