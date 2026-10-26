#include "ExperimentTile.h"
#include "ExperimentService.h"
#include "Manifest.h"
#include "Theme.h"

#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace {

// Classic "download" glyph: an arrow pointing into a tray. Drawn in code so no
// image asset (and no emoji) is required.
QIcon makeDownloadIcon(const QColor &color)
{
    const int s = 40;
    QPixmap pm(s, s);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(color, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    const qreal cx = s / 2.0;
    p.drawLine(QPointF(cx, 8.5), QPointF(cx, 25.0));       // shaft
    p.drawLine(QPointF(cx - 6.5, 18.5), QPointF(cx, 25.0)); // head (left)
    p.drawLine(QPointF(cx + 6.5, 18.5), QPointF(cx, 25.0)); // head (right)
    p.drawLine(QPointF(9.0, 31.5), QPointF(31.0, 31.5));    // tray
    return QIcon(pm);
}

// "Play" triangle, shown once the experiment is downloaded.
QIcon makePlayIcon(const QColor &color)
{
    const int s = 40;
    QPixmap pm(s, s);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.moveTo(13.5, 9.0);
    path.lineTo(31.0, 20.0);
    path.lineTo(13.5, 31.0);
    path.closeSubpath();
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawPath(path);
    return QIcon(pm);
}

} // namespace

ExperimentTile::ExperimentTile(const Experiment &e, ExperimentService *service, QWidget *parent)
    : QFrame(parent)
    , m_exp(e)
    , m_service(service)
{
    setObjectName(QStringLiteral("card"));

    m_downloaded = e.downloadable && m_service->isDownloaded(e.projectPath);

    auto *body = new QVBoxLayout(this);
    body->setContentsMargins(20, 18, 20, 16);
    body->setSpacing(6);

    // Title row + favorite badge.
    auto *topRow = new QHBoxLayout;
    auto *name = new QLabel(e.name, this);
    name->setObjectName(QStringLiteral("expName"));
    topRow->addWidget(name, 1);
    if (e.favorite) {
        auto *fav = new QLabel(QStringLiteral("★ Favorite"), this);
        fav->setObjectName(QStringLiteral("badgeFav"));
        topRow->addWidget(fav);
    }
    body->addLayout(topRow);

    // Description.
    auto *desc = new QLabel(e.description.isEmpty() ? QStringLiteral("—") : e.description, this);
    desc->setObjectName(QStringLiteral("expDesc"));
    desc->setWordWrap(true);
    desc->setMaximumHeight(44);
    body->addWidget(desc);

    // Meta line: language • framework • engine.
    QStringList metaParts;
    if (!e.language.isEmpty())
        metaParts << e.language;
    if (!e.framework.isEmpty())
        metaParts << e.framework;
    if (!e.engine.isEmpty())
        metaParts << e.engine;
    auto *meta = new QLabel(metaParts.isEmpty() ? QString()
                                                : metaParts.join(QStringLiteral(" • ")), this);
    meta->setObjectName(QStringLiteral("meta"));
    body->addWidget(meta);

    // Manifest summary (YLEDMETA.yaml): run command + download source.
    m_manifestLabel = new QLabel(this);
    m_manifestLabel->setObjectName(QStringLiteral("mono"));
    m_manifestLabel->setWordWrap(true);
    m_manifestLabel->hide();
    body->addWidget(m_manifestLabel);

    // Inline download progress (hidden until a download starts).
    m_progress = new QProgressBar(this);
    m_progress->setObjectName(QStringLiteral("tileProgress"));
    m_progress->setRange(0, 100);
    m_progress->setTextVisible(false);
    m_progress->hide();
    body->addWidget(m_progress);

    // Bottom row: status badge + download icon + View Details.
    auto *bottomRow = new QHBoxLayout;
    auto *status = new QLabel(e.status.isEmpty() ? QStringLiteral("Unknown") : e.status, this);
    status->setObjectName(QStringLiteral("badgeStatus"));
    bottomRow->addWidget(status);
    bottomRow->addStretch(1);

    m_iconBtn = new QToolButton(this);
    m_iconBtn->setObjectName(QStringLiteral("iconBtn"));
    m_iconBtn->setCursor(Qt::PointingHandCursor);
    m_iconBtn->setIconSize(QSize(20, 20));
    m_iconBtn->setFixedSize(38, 38);
    connect(m_iconBtn, &QToolButton::clicked, this, &ExperimentTile::onIconClicked);
    bottomRow->addWidget(m_iconBtn);

    auto *btn = new QPushButton(QStringLiteral("View Details"), this);
    btn->setObjectName(QStringLiteral("btnPrimary"));
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, [this]() {
        emit openDetails(m_exp.id);
    });
    bottomRow->addWidget(btn);

    body->addLayout(bottomRow);

    connect(m_service, &ExperimentService::downloadProgress,
            this, [this](const QString &projectPath, double percent) {
        if (projectPath != m_exp.projectPath)
            return;
        m_percent = percent;
        if (m_progress->isVisible())
            m_progress->setValue(static_cast<int>(percent));
    });

    connect(m_service, &ExperimentService::downloadFinished,
            this, [this](const QString &projectPath, bool ok, const QString &error) {
        if (projectPath != m_exp.projectPath)
            return;
        m_downloading = false;
        if (ok)
            m_downloaded = m_service->isDownloaded(m_exp.projectPath);
        else
            m_error = error;
        refresh();
    });

    connect(m_service, &ExperimentService::manifestChanged,
            this, [this](const QString &projectPath) {
        if (projectPath == m_exp.projectPath)
            refresh();
    });

    refresh();
}

void ExperimentTile::refresh()
{
    // Manifest summary line (works for downloadable and non-downloadable alike).
    const Manifest mf = m_service->manifestFor(m_exp.projectPath);
    if (mf.valid) {
        QStringList bits;
        const QString run = mf.runCommand();
        if (!run.isEmpty())
            bits << QStringLiteral("run: %1").arg(run);
        if (mf.useTgz)
            bits << QStringLiteral("tgz");
        else if (mf.useGitDir)
            bits << QStringLiteral("git-dir");
        if (bits.isEmpty()) {
            m_manifestLabel->hide();
        } else {
            m_manifestLabel->setText(bits.join(QStringLiteral("     ")));
            m_manifestLabel->show();
        }
    } else {
        m_manifestLabel->hide();
    }

    if (!m_exp.downloadable) {
        m_iconBtn->hide();
        m_progress->hide();
        return;
    }

    m_iconBtn->show();

    if (m_downloading) {
        m_iconBtn->setEnabled(false);
        m_iconBtn->setIcon(makeDownloadIcon(Theme::Yellow.darker(140)));
        m_iconBtn->setToolTip(QStringLiteral("Downloading… %1%").arg(m_percent, 0, 'f', 0));
        m_progress->setValue(static_cast<int>(m_percent));
        m_progress->show();
    } else if (m_downloaded) {
        m_iconBtn->setEnabled(true);
        m_iconBtn->setIcon(makePlayIcon(Theme::Success));
        m_iconBtn->setToolTip(QStringLiteral("Run"));
        m_progress->hide();
    } else {
        m_iconBtn->setEnabled(true);
        m_iconBtn->setIcon(makeDownloadIcon(Theme::Yellow));
        m_iconBtn->setToolTip(m_error.isEmpty() ? QStringLiteral("Download")
                                                : QStringLiteral("Download (last error: %1)").arg(m_error));
        m_progress->hide();
    }
}

void ExperimentTile::onIconClicked()
{
    if (m_downloaded) {
        run();
        return;
    }
    startDownload();
}

void ExperimentTile::startDownload()
{
    if (m_downloading || m_service->isDownloading())
        return;
    m_downloading = true;
    m_error.clear();
    m_percent = 0;
    refresh();
    m_service->downloadExperiment(m_exp.projectPath);
}

void ExperimentTile::run()
{
    QString error;
    m_service->runExperiment(m_exp.projectPath, &error);
    if (!error.isEmpty()) {
        m_error = error;
        m_iconBtn->setToolTip(QStringLiteral("Run failed: %1").arg(error));
    }
}