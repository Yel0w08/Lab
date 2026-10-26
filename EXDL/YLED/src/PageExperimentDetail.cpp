#include "PageExperimentDetail.h"

#include "ExperimentService.h"
#include "Manifest.h"

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

PageExperimentDetail::PageExperimentDetail(ExperimentService *service, QWidget *parent)
    : QWidget(parent)
    , m_service(service)
{
    setObjectName(QStringLiteral("page"));

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(48, 44, 48, 40);
    root->setSpacing(14);

    auto *backBtn = new QPushButton(QStringLiteral("Back"), this);
    backBtn->setObjectName(QStringLiteral("btnSecondary"));
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &PageExperimentDetail::back);
    root->addWidget(backBtn, 0, Qt::AlignLeft);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("statusError"));
    m_statusLabel->setWordWrap(true);
    root->addWidget(m_statusLabel);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    m_contentHost = new QWidget(scroll);
    m_contentHost->setObjectName(QStringLiteral("listHost"));
    m_body = new QVBoxLayout(m_contentHost);
    m_body->setContentsMargins(0, 4, 12, 8);
    m_body->setSpacing(0);
    scroll->setWidget(m_contentHost);
    root->addWidget(scroll, 1);

    connect(m_service, &ExperimentService::downloadProgress,
            this, [this](const QString &projectPath, double percent) {
        if (!m_hasExp || projectPath != m_exp.projectPath)
            return;
        m_progress = percent;
        render();
    });

    connect(m_service, &ExperimentService::downloadFinished,
            this, [this](const QString &projectPath, bool ok, const QString &error) {
        if (!m_hasExp || projectPath != m_exp.projectPath)
            return;
        m_downloading = false;
        m_status = ok ? QString() : QStringLiteral("Download failed: %1").arg(error);
        if (ok)
            m_downloaded = m_service->isDownloaded(m_exp.projectPath);
        render();
    });

    connect(m_service, &ExperimentService::manifestChanged,
            this, [this](const QString &projectPath) {
        if (m_hasExp && projectPath == m_exp.projectPath)
            render();
    });
}

void PageExperimentDetail::showExperiment(const Experiment &e)
{
    m_exp = e;
    m_hasExp = true;
    m_downloading = false;
    m_status.clear();
    m_downloaded = m_exp.downloadable && m_service->isDownloaded(m_exp.projectPath);
    render();
}

static void clearLayout(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        if (QLayout *sub = item->layout())
            clearLayout(sub);
        delete item;
    }
}

void PageExperimentDetail::render()
{
    clearLayout(m_body);
    m_statusLabel->setText(m_status);

    if (!m_hasExp) {
        auto *msg = new QLabel(QStringLiteral("Select an experiment from the list."), m_contentHost);
        msg->setObjectName(QStringLiteral("lead"));
        m_body->addWidget(msg);
        return;
    }

    auto *card = new QFrame(m_contentHost);
    card->setObjectName(QStringLiteral("card"));
    auto *body = new QVBoxLayout(card);
    body->setContentsMargins(24, 22, 24, 22);
    body->setSpacing(10);

    // Header: name + favorite badge.
    auto *topRow = new QHBoxLayout;
    auto *name = new QLabel(m_exp.name, card);
    name->setObjectName(QStringLiteral("expName"));
    name->setStyleSheet(QStringLiteral("font-size: 20px;"));
    topRow->addWidget(name);
    topRow->addStretch(1);
    if (m_exp.favorite) {
        auto *fav = new QLabel(QStringLiteral("★ Favorite"), card);
        fav->setObjectName(QStringLiteral("badgeFav"));
        topRow->addWidget(fav);
    }
    body->addLayout(topRow);

    auto *desc = new QLabel(m_exp.description, card);
    desc->setObjectName(QStringLiteral("expDesc"));
    desc->setWordWrap(true);
    body->addWidget(desc);

    // Meta: language • framework • engine.
    QStringList metaParts;
    if (!m_exp.language.isEmpty())   metaParts << m_exp.language;
    if (!m_exp.framework.isEmpty())  metaParts << m_exp.framework;
    if (!m_exp.engine.isEmpty())     metaParts << m_exp.engine;
    if (!metaParts.isEmpty()) {
        auto *meta = new QLabel(metaParts.join(QStringLiteral(" • ")), card);
        meta->setObjectName(QStringLiteral("meta"));
        body->addWidget(meta);
    }

    // Status badge + tags.
    auto *badgeRow = new QHBoxLayout;
    auto *status = new QLabel(m_exp.status.isEmpty() ? QStringLiteral("Unknown") : m_exp.status, card);
    status->setObjectName(QStringLiteral("badgeStatus"));
    badgeRow->addWidget(status, 0, Qt::AlignLeft);
    badgeRow->addStretch(1);
    for (const QString &tag : m_exp.tags) {
        auto *t = new QLabel(tag, card);
        t->setObjectName(QStringLiteral("badgeTag"));
        badgeRow->addWidget(t);
    }
    body->addLayout(badgeRow);

    // Notes.
    if (!m_exp.notes.isEmpty()) {
        auto *noteTitle = new QLabel(QStringLiteral("Notes"), card);
        noteTitle->setObjectName(QStringLiteral("secTitle"));
        body->addWidget(noteTitle);
        auto *notes = new QLabel(m_exp.notes, card);
        notes->setObjectName(QStringLiteral("expDesc"));
        notes->setWordWrap(true);
        body->addWidget(notes);
    }

    // Dates.
    const QString created = m_exp.createdAt.isValid()
        ? m_exp.createdAt.toString(QStringLiteral("yyyy-MM-dd HH:mm"))
        : QStringLiteral("—");
    const QString modified = m_exp.lastModified.isValid()
        ? m_exp.lastModified.toString(QStringLiteral("yyyy-MM-dd HH:mm"))
        : QStringLiteral("—");
    body->addSpacing(6);
    auto *dates = new QLabel(QStringLiteral("Created: %1    |    Last modified: %2")
                                 .arg(created, modified), card);
    dates->setObjectName(QStringLiteral("meta"));
    body->addWidget(dates);

    // Manifest (YLEDMETA.yaml) summary.
    const Manifest mf = m_service->manifestFor(m_exp.projectPath);
    if (mf.valid) {
        auto *metaTitle = new QLabel(QStringLiteral("Metadata (YLEDMETA.yaml)"), card);
        metaTitle->setObjectName(QStringLiteral("secTitle"));
        body->addWidget(metaTitle);

        QStringList lines;
        const QString run = mf.runCommand();
        if (!run.isEmpty())
            lines << QStringLiteral("run: %1").arg(run);
        const QString init = mf.initCommand();
        if (!init.isEmpty())
            lines << QStringLiteral("init: %1").arg(init);
        if (mf.useTgz && !mf.tgzUrl().isEmpty())
            lines << QStringLiteral("dl: tgz  %1").arg(mf.tgzUrl());
        else if (mf.useGitDir && !mf.gitDirUrl().isEmpty())
            lines << QStringLiteral("dl: git  %1").arg(mf.gitDirUrl());
        else if (!mf.tgzUrl().isEmpty())
            lines << QStringLiteral("dl: tgz  %1").arg(mf.tgzUrl());
        else if (!mf.gitDirUrl().isEmpty())
            lines << QStringLiteral("dl: git  %1").arg(mf.gitDirUrl());

        if (!lines.isEmpty()) {
            auto *ml = new QLabel(lines.join(QLatin1Char('\n')), card);
            ml->setObjectName(QStringLiteral("mono"));
            ml->setWordWrap(true);
            body->addWidget(ml);
        }
    }

    // Separator.
    auto *sep = new QFrame(card);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName(QStringLiteral("sep"));
    body->addWidget(sep);

    // Action row.
    auto *actionRow = new QHBoxLayout;
    if (!m_exp.downloadable) {
        auto *na = new QLabel(QStringLiteral("Not available for download"), card);
        na->setObjectName(QStringLiteral("meta"));
        actionRow->addWidget(na);
    } else if (m_downloaded) {
        auto *runBtn = new QPushButton(QStringLiteral("Run"), card);
        runBtn->setObjectName(QStringLiteral("btnRun"));
        runBtn->setCursor(Qt::PointingHandCursor);
        connect(runBtn, &QPushButton::clicked, this, &PageExperimentDetail::run);
        actionRow->addWidget(runBtn);
    } else if (m_downloading) {
        auto *prog = new QProgressBar(card);
        prog->setRange(0, 100);
        prog->setValue(static_cast<int>(m_progress));
        prog->setMinimumWidth(220);
        auto *pct = new QLabel(QStringLiteral("%1%").arg(m_progress, 0, 'f', 0), card);
        pct->setObjectName(QStringLiteral("meta"));
        actionRow->addWidget(prog, 1);
        actionRow->addWidget(pct);
    } else {
        auto *dlBtn = new QPushButton(QStringLiteral("Download"), card);
        dlBtn->setObjectName(QStringLiteral("btnPrimary"));
        dlBtn->setCursor(Qt::PointingHandCursor);
        connect(dlBtn, &QPushButton::clicked, this, &PageExperimentDetail::startDownload);
        actionRow->addWidget(dlBtn);
    }
    body->addLayout(actionRow);

    m_body->addWidget(card);
    m_body->addStretch(1);
}

void PageExperimentDetail::startDownload()
{
    if (!m_hasExp || m_downloading || m_service->isDownloading())
        return;
    m_downloading = true;
    m_status.clear();
    m_progress = 0;
    render();
    m_service->downloadExperiment(m_exp.projectPath);
}

void PageExperimentDetail::run()
{
    if (!m_hasExp)
        return;
    QString error;
    m_service->runExperiment(m_exp.projectPath, &error);
    m_status = error;
    render();
}