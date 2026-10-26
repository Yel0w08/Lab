#include "MainWindow.h"

#include "ExperimentService.h"
#include "NavBar.h"
#include "PageExperiments.h"
#include "PageExperimentDetail.h"
#include "PageSettings.h"
#include "Experiment.h"

#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QHBoxLayout>

// Page indices inside the stacked widget.
enum Page { PageIndexExperiments = 0, PageIndexDetail = 1, PageIndexSettings = 2 };

MainWindow::MainWindow()
{
    setWindowTitle(QStringLiteral("YLED — Yel0w's Lab Experiment Donloader"));
    resize(1120, 720);
    setMinimumSize(900, 600);

    m_service = new ExperimentService(this);

    auto *root = new QWidget(this);
    root->setObjectName(QStringLiteral("root"));
    setCentralWidget(root);

    auto *grid = new QGridLayout(root);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);

    m_nav = new NavBar(root);
    grid->addWidget(m_nav, 0, 0);

    m_stack = new QStackedWidget(root);
    m_stack->setObjectName(QStringLiteral("mainStack"));
    grid->addWidget(m_stack, 0, 1);

    m_experiments = new PageExperiments(m_service);
    m_detail = new PageExperimentDetail(m_service);
    m_settings = new PageSettings(m_service);

    m_stack->addWidget(m_experiments);   // 0
    m_stack->addWidget(m_detail);        // 1
    m_stack->addWidget(m_settings);      // 2

    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(0, 1);

    connect(m_nav, &NavBar::navigate, this, &MainWindow::goTo);
    connect(m_experiments, &PageExperiments::openExperiment, this, &MainWindow::openExperiment);
    connect(m_detail, &PageExperimentDetail::back, this, [this]() {
        goTo(PageIndexExperiments);
    });

    goTo(PageIndexExperiments);
}

void MainWindow::goTo(int index)
{
    if (index == PageIndexExperiments)
        m_experiments->reload(false);

    if (index >= 0 && index < m_stack->count())
        m_stack->setCurrentIndex(index);

    m_nav->setCurrent(index);
}

void MainWindow::openExperiment(int id)
{
    const Experiment e = m_service->experimentById(id);
    m_detail->showExperiment(e);
    m_stack->setCurrentIndex(PageIndexDetail);
    m_nav->setCurrent(PageIndexDetail);
}
