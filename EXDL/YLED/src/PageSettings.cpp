#include "PageSettings.h"

#include "ExperimentService.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QStyle>

PageSettings::PageSettings(ExperimentService *service, QWidget *parent)
    : QWidget(parent)
    , m_service(service)
{
    setObjectName(QStringLiteral("page"));

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(48, 44, 48, 40);
    root->setSpacing(18);

    auto *head = new QLabel(QStringLiteral("Settings"), this);
    head->setObjectName(QStringLiteral("h1"));
    root->addWidget(head);

    // Database card.
    auto *dbCard = new QFrame(this);
    dbCard->setObjectName(QStringLiteral("panel"));
    auto *dbBody = new QVBoxLayout(dbCard);
    dbBody->setContentsMargins(22, 20, 22, 20);
    dbBody->setSpacing(10);

    auto *dbTitle = new QLabel(QStringLiteral("Database"), dbCard);
    dbTitle->setObjectName(QStringLiteral("secTitle"));
    auto *dbText = new QLabel(QStringLiteral(
        "The experiment database is hosted on GitHub and cached locally for faster loading."),
        dbCard);
    dbText->setObjectName(QStringLiteral("cardSub"));
    dbText->setWordWrap(true);

    auto *btnRow = new QHBoxLayout;
    auto *clearBtn = new QPushButton(QStringLiteral("Clear Cache"), dbCard);
    clearBtn->setObjectName(QStringLiteral("btnDanger"));
    clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, this, &PageSettings::clearCache);

    auto *refreshBtn = new QPushButton(QStringLiteral("Refresh Database"), dbCard);
    refreshBtn->setObjectName(QStringLiteral("btnSecondary"));
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &PageSettings::forceRefresh);

    btnRow->addWidget(clearBtn);
    btnRow->addWidget(refreshBtn);
    btnRow->addStretch(1);

    m_statusLabel = new QLabel(dbCard);
    m_statusLabel->setWordWrap(true);

    dbBody->addWidget(dbTitle);
    dbBody->addWidget(dbText);
    dbBody->addLayout(btnRow);
    dbBody->addWidget(m_statusLabel);
    root->addWidget(dbCard);

    // Manifests card.
    auto *metaCard = new QFrame(this);
    metaCard->setObjectName(QStringLiteral("panel"));
    auto *metaBody = new QVBoxLayout(metaCard);
    metaBody->setContentsMargins(22, 20, 22, 20);
    metaBody->setSpacing(10);

    auto *metaTitle = new QLabel(QStringLiteral("Experiment Manifests"), metaCard);
    metaTitle->setObjectName(QStringLiteral("secTitle"));
    auto *metaText = new QLabel(QStringLiteral(
        "Each experiment ships a YLEDMETA.yaml that declares its run/init commands and "
        "where to download it from. Manifests are fetched from the Lab repository and "
        "cached locally."), metaCard);
    metaText->setObjectName(QStringLiteral("cardSub"));
    metaText->setWordWrap(true);

    auto *metaBtnRow = new QHBoxLayout;
    auto *metaRefreshBtn = new QPushButton(QStringLiteral("Refresh Manifests"), metaCard);
    metaRefreshBtn->setObjectName(QStringLiteral("btnSecondary"));
    metaRefreshBtn->setCursor(Qt::PointingHandCursor);
    connect(metaRefreshBtn, &QPushButton::clicked, this, &PageSettings::refreshManifests);
    metaBtnRow->addWidget(metaRefreshBtn);
    metaBtnRow->addStretch(1);

    metaBody->addWidget(metaTitle);
    metaBody->addWidget(metaText);
    metaBody->addLayout(metaBtnRow);
    root->addWidget(metaCard);

    // About card.
    auto *aboutCard = new QFrame(this);
    aboutCard->setObjectName(QStringLiteral("panel"));
    auto *aboutBody = new QVBoxLayout(aboutCard);
    aboutBody->setContentsMargins(22, 20, 22, 20);
    aboutBody->setSpacing(6);

    auto *aboutTitle = new QLabel(QStringLiteral("About"), aboutCard);
    aboutTitle->setObjectName(QStringLiteral("secTitle"));
    auto *about1 = new QLabel(QStringLiteral("YLED — Yel0w's Lab Experiment Donloader"), aboutCard);
    about1->setObjectName(QStringLiteral("cardSub"));
    auto *about2 = new QLabel(QStringLiteral("Experiment management and launcher. Built with C++ / Qt."), aboutCard);
    about2->setObjectName(QStringLiteral("cardSub"));

    aboutBody->addWidget(aboutTitle);
    aboutBody->addWidget(about1);
    aboutBody->addWidget(about2);
    root->addWidget(aboutCard);

    root->addStretch(1);

    // A single persistent listener for database refresh results.
    connect(m_service, &ExperimentService::experimentsChanged, this,
            [this](bool ok, const QString &error) {
        if (!m_waitingForRefresh)
            return;
        m_waitingForRefresh = false;
        if (ok)
            setStatus(QStringLiteral("Database refreshed. %1 experiments loaded.")
                          .arg(m_service->experiments().size()), "success");
        else
            setStatus(QStringLiteral("Failed to refresh: %1").arg(error), "error");
    });
}

void PageSettings::setStatus(const QString &text, const char *kind)
{
    m_statusLabel->setText(text);
    m_statusLabel->setProperty("kind", QString::fromLatin1(kind));
    m_statusLabel->style()->unpolish(m_statusLabel);
    m_statusLabel->style()->polish(m_statusLabel);
}

void PageSettings::clearCache()
{
    m_service->clearCache();
    setStatus(QStringLiteral("Cache cleared. Database will be re-downloaded on next load."),
              "success");
}

void PageSettings::forceRefresh()
{
    m_waitingForRefresh = true;
    setStatus(QStringLiteral("Refreshing…"), "info");
    // Re-read from the cached database file (same behaviour as Yel0w.Lab).
    m_service->requestExperiments(true);
}

void PageSettings::refreshManifests()
{
    m_waitingForRefresh = true;
    setStatus(QStringLiteral("Refreshing manifests…"), "info");
    // Reloading the database also re-fetches every experiment's YLEDMETA.yaml.
    m_service->requestExperiments(true);
}