#pragma once

#include <QMainWindow>

class QStackedWidget;
class ExperimentService;
class NavBar;
class PageExperiments;
class PageExperimentDetail;
class PageSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private:
    void goTo(int index);
    void openExperiment(int id);

    ExperimentService *m_service = nullptr;
    NavBar *m_nav = nullptr;
    QStackedWidget *m_stack = nullptr;
    PageExperiments *m_experiments = nullptr;
    PageExperimentDetail *m_detail = nullptr;
    PageSettings *m_settings = nullptr;
};
