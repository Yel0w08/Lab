#pragma once

#include <QWidget>

class QLabel;
class QVBoxLayout;
class QScrollArea;
class ExperimentService;

// Experiments page: lists experiments from the Lab database.
class PageExperiments : public QWidget
{
    Q_OBJECT
public:
    explicit PageExperiments(ExperimentService *service, QWidget *parent = nullptr);

    void reload(bool force = false);

signals:
    void openExperiment(int id);

private:
    void rebuild();

    ExperimentService *m_service;
    QVBoxLayout *m_listLayout;
    QLabel *m_statusLabel;
    bool m_lastLoadFailed = false;
};