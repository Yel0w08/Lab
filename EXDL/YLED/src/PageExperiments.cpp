#include "PageExperiments.h"

#include "Experiment.h"
#include "ExperimentService.h"
#include "ExperimentTile.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

PageExperiments::PageExperiments(ExperimentService *service, QWidget *parent)
    : QWidget(parent)
    , m_service(service)
{
    setObjectName(QStringLiteral("page"));

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(48, 44, 48, 40);
    root->setSpacing(14);

    auto *head = new QLabel(QStringLiteral("Experiments"), this);
    head->setObjectName(QStringLiteral("h1"));
    root->addWidget(head);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("statusInfo"));
    root->addWidget(m_statusLabel);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto *host = new QWidget(scroll);
    host->setObjectName(QStringLiteral("listHost"));
    m_listLayout = new QVBoxLayout(host);
    m_listLayout->setContentsMargins(0, 4, 8, 8);
    m_listLayout->setSpacing(12);
    scroll->setWidget(host);
    root->addWidget(scroll, 1);

    connect(m_service, &ExperimentService::experimentsChanged,
            this, [this](bool ok, const QString &error) {
        m_lastLoadFailed = !ok;
        if (!ok)
            m_statusLabel->setText(tr("Error: %1").arg(error));
        else
            m_statusLabel->setText(tr("%1 experiment(s) in the lab.")
                                       .arg(m_service->experiments().size()));
        rebuild();
    });
}

void PageExperiments::reload(bool force)
{
    m_lastLoadFailed = false;
    m_statusLabel->setText(QStringLiteral("Loading…"));
    m_service->requestExperiments(force);
}

void PageExperiments::rebuild()
{
    // Clear the existing list.
    while (QLayoutItem *item = m_listLayout->takeAt(0)) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        delete item;
    }

    if (m_lastLoadFailed)
        return;

    const QList<Experiment> &list = m_service->experiments();
    if (list.isEmpty()) {
        auto *empty = new QLabel(QStringLiteral("No experiments found."), this);
        empty->setObjectName(QStringLiteral("lead"));
        m_listLayout->addWidget(empty);
        return;
    }

    for (const Experiment &e : list) {
        auto *tile = new ExperimentTile(e, m_service, this);
        connect(tile, &ExperimentTile::openDetails, this, &PageExperiments::openExperiment);
        m_listLayout->addWidget(tile);
    }

    m_listLayout->addStretch(1);
}
