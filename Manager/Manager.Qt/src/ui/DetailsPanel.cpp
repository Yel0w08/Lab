#include "ui/DetailsPanel.h"

#include "core/Theme.h"
#include "ui/Blueprint.h"
#include "ui/Chips.h"

#include <QDesktopServices>
#include <QEvent>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QUrl>
#include <QVBoxLayout>

/** Tag chips wrapped on as many rows as needed. */
class TagChipsWidget : public QWidget
{
public:
    explicit TagChipsWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    void setTags(const QStringList &tags)
    {
        m_tags = tags;
        updateGeometry();
        update();
    }

    QSize sizeHint() const override { return QSize(120, heightForWidth(120)); }
    QSize minimumSizeHint() const override { return QSize(60, heightForWidth(60)); }

    bool hasHeightForWidth() const override { return true; }

    int heightForWidth(int width) const override
    {
        if (m_tags.isEmpty())
            return 0;
        const int height = Chips::heightForWidth(m_tags, chipFont(), width, 6, 24, 9);
        return height > 0 ? height + 4 : 0;
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (m_tags.isEmpty())
            return;

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        Chips::paintWrapped(painter, m_tags, rect().adjusted(0, 1, -2, -1), chipFont(),
                            Theme::current().dark, 6, 24, 9);
    }

private:
    QFont chipFont() const
    {
        return Theme::monoFont(Theme::scaledFont(this->font(), -1));
    }

    QStringList m_tags;
};

namespace {

QString pillStyle(const QColor &color, bool dark)
{
    const auto rgb = [](const QColor &c) {
        return QStringLiteral("%1,%2,%3").arg(c.red()).arg(c.green()).arg(c.blue());
    };

    const double backgroundAlpha = dark ? 0.17 : 0.13;
    const double borderAlpha = dark ? 0.40 : 0.30;

    return QStringLiteral(
               "QLabel {"
               "  font-family: \"%1\";"
               "  background: rgba(%2,%3);"
               "  color: rgb(%4);"
               "  border: 1px solid rgba(%5,%6);"
               "  border-radius: 0px;"
               "  padding: 4px 12px;"
               "  font-size: 11px;"
               "  font-weight: 700;"
               "  letter-spacing: 0.6px;"
               "}")
        .arg(Theme::current().mono)
        .arg(rgb(color))
        .arg(backgroundAlpha, 0, 'f', 2)
        .arg(rgb(color))
        .arg(rgb(color))
        .arg(borderAlpha, 0, 'f', 2);
}

QString formatDateTime(const QDateTime &dateTime)
{
    if (!dateTime.isValid())
        return QStringLiteral("—");
    return dateTime.toString(QStringLiteral("d MMM yyyy, hh:mm"));
}

} // namespace

DetailsPanel::DetailsPanel(QWidget *parent)
    : QWidget(parent)
{
    const auto makeMono = [](QWidget *widget) {
        widget->setFont(Theme::monoFont(widget->font()));
    };

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);

    auto *card = new LabFrame;
    card->setObjectName(QStringLiteral("card"));

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(4, 14, 4, 14);
    cardLayout->setSpacing(0);

    auto *scroll = new QScrollArea;
    scroll->setObjectName(QStringLiteral("detailsScroll"));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *inner = new QWidget;
    inner->setObjectName(QStringLiteral("detailsInner"));
    auto *innerLayout = new QVBoxLayout(inner);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(0);

    // ---- empty state -------------------------------------------------------
    m_placeholder = new QWidget;
    auto *placeholderLayout = new QVBoxLayout(m_placeholder);
    placeholderLayout->setContentsMargins(16, 8, 16, 8);
    placeholderLayout->addStretch();

    auto *glyph = new QLabel(QStringLiteral("◇"));
    glyph->setObjectName(QStringLiteral("overlayGlyph"));
    glyph->setAlignment(Qt::AlignCenter);

    auto *placeholderTitle = new QLabel(QStringLiteral("No selection"));
    placeholderTitle->setObjectName(QStringLiteral("overlayTitle"));
    placeholderTitle->setAlignment(Qt::AlignCenter);

    auto *placeholderText = new QLabel(
        QStringLiteral("Pick an experiment in the list to see its details here."));
    placeholderText->setObjectName(QStringLiteral("overlayText"));
    placeholderText->setAlignment(Qt::AlignCenter);
    placeholderText->setWordWrap(true);

    placeholderLayout->addWidget(glyph);
    placeholderLayout->addSpacing(6);
    placeholderLayout->addWidget(placeholderTitle);
    placeholderLayout->addSpacing(4);
    placeholderLayout->addWidget(placeholderText);
    placeholderLayout->addStretch();
    innerLayout->addWidget(m_placeholder);

    // ---- details -----------------------------------------------------------
    m_content = new QWidget;
    auto *layout = new QVBoxLayout(m_content);
    layout->setContentsMargins(16, 4, 16, 16);
    layout->setSpacing(12);

    auto *headerRow = new QHBoxLayout;
    m_statusPill = new QLabel;
    m_statusHint = new QLabel;
    m_statusHint->setObjectName(QStringLiteral("hint"));
    makeMono(m_statusHint);
    m_star = new QPushButton(QStringLiteral("☆"));
    m_star->setObjectName(QStringLiteral("detailsStar"));
    m_star->setCursor(Qt::PointingHandCursor);
    m_star->setToolTip(tr("Toggle favourite (Ctrl+D)"));
    m_star->setFixedSize(34, 34);
    m_star->setFocusPolicy(Qt::NoFocus);

    headerRow->addWidget(m_statusPill, 0, Qt::AlignLeft);
    headerRow->addStretch();
    headerRow->addWidget(m_star);

    m_name = new QLabel;
    QFont nameFont = Theme::scaledFont(m_name->font(), 7);
    nameFont.setWeight(QFont::DemiBold);
    m_name->setFont(nameFont);
    m_name->setWordWrap(true);

    m_tech = new QLabel;
    m_tech->setObjectName(QStringLiteral("muted"));
    makeMono(m_tech);

    auto *head = new QVBoxLayout;
    head->setSpacing(6);
    head->addLayout(headerRow);
    head->addWidget(m_name);
    head->addWidget(m_tech);
    head->addWidget(m_statusHint);

    // description
    m_description = new QLabel;
    m_description->setWordWrap(true);
    m_description->setObjectName(QStringLiteral("muted"));
    m_description->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addLayout(head);
    layout->addWidget(section(QStringLiteral("DESCRIPTION"), m_description));

    // tags
    m_tags = new TagChipsWidget;
    layout->addWidget(section(QStringLiteral("TAGS"), m_tags));

    // notes
    m_notes = new QLabel;
    m_notes->setWordWrap(true);
    m_notes->setObjectName(QStringLiteral("muted"));
    m_notes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(section(QStringLiteral("NOTES"), m_notes));

    // project
    auto *pathRow = new QHBoxLayout;
    m_path = new QLabel;
    m_path->setWordWrap(true);
    m_path->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_path->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_openButton = new QPushButton(QStringLiteral("Open"));
    m_openButton->setObjectName(QStringLiteral("softButton"));
    m_openButton->setCursor(Qt::PointingHandCursor);
    m_openButton->setToolTip(tr("Open the project folder"));
    makeMono(m_path);

    pathRow->addWidget(m_path, 1);
    pathRow->addWidget(m_openButton, 0, Qt::AlignTop);
    layout->addWidget(section(QStringLiteral("PROJECT"), [pathRow]() {
        auto *widget = new QWidget;
        auto *box = new QVBoxLayout(widget);
        box->setContentsMargins(0, 0, 0, 0);
        box->setSpacing(6);
        box->addLayout(pathRow);
        return widget;
    }()));

    // dates
    auto *dates = new QWidget;
    auto *datesLayout = new QVBoxLayout(dates);
    datesLayout->setContentsMargins(0, 0, 0, 0);
    datesLayout->setSpacing(4);
    m_created = new QLabel;
    m_created->setObjectName(QStringLiteral("muted"));
    m_modified = new QLabel;
    m_modified->setObjectName(QStringLiteral("muted"));
    makeMono(m_created);
    makeMono(m_modified);
    datesLayout->addWidget(m_created);
    datesLayout->addWidget(m_modified);
    layout->addWidget(section(QStringLiteral("TIMELINE"), dates));

    layout->addStretch();

    m_editButton = new QPushButton(QStringLiteral("Edit experiment"));
    m_editButton->setObjectName(QStringLiteral("primaryButton"));
    m_editButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_editButton);

    innerLayout->addWidget(m_content);
    scroll->setWidget(inner);

    cardLayout->addWidget(scroll);
    outer->addWidget(card);

    // Corner-targeting brackets, always painted above the card content.
    m_brackets = new BracketOverlay(card);
    m_brackets->setGeometry(card->rect());
    m_brackets->raise();
    card->installEventFilter(this);

    // ---- signals -----------------------------------------------------------
    connect(m_editButton, &QPushButton::clicked, this, [this]() {
        if (m_hasExperiment)
            emit editRequested(m_id);
    });

    connect(m_star, &QPushButton::clicked, this, [this]() {
        if (m_hasExperiment)
            emit favoriteToggled(m_id);
    });

    connect(m_openButton, &QPushButton::clicked, this, [this]() {
        if (!m_hasExperiment)
            return;
        const QString path = m_currentPath;
        if (path.isEmpty())
            return;
        if (QFileInfo::exists(path))
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });

    clear();
}

QWidget *DetailsPanel::section(const QString &title, QWidget *body)
{
    auto *widget = new QWidget;
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    auto *label = new QLabel(title);
    label->setObjectName(QStringLiteral("cardSection"));

    layout->addWidget(label);
    layout->addWidget(body);
    return widget;
}

void DetailsPanel::refreshFavoriteStar(const Experiment &experiment)
{
    const Theme::Palette &palette = Theme::current();
    const QString color = experiment.favorite ? palette.star : palette.textFaint;
    const QString hover = experiment.favorite ? palette.star : palette.textDim;

    m_star->setText(experiment.favorite ? QStringLiteral("★") : QStringLiteral("☆"));
    m_star->setStyleSheet(QStringLiteral(
                              "QPushButton#detailsStar {"
                              "  background: transparent;"
                              "  border: none;"
                              "  border-radius: 0px;"
                              "  font-size: 20px;"
                              "  font-weight: 700;"
                              "  color: %1;"
                              "  padding: 0px;"
                              "}"
                              "QPushButton#detailsStar:hover { background: %2; color: %3; }")
                              .arg(color, palette.surfaceHover, hover));
}

void DetailsPanel::clear()
{
    m_id = 0;
    m_hasExperiment = false;
    m_currentPath.clear();

    m_content->setVisible(false);
    m_placeholder->setVisible(true);
    m_openButton->setEnabled(false);
}

void DetailsPanel::showExperiment(const Experiment *experiment)
{
    if (!experiment || experiment->id == 0) {
        clear();
        return;
    }

    const Theme::Palette &palette = Theme::current();

    m_id = experiment->id;
    m_hasExperiment = true;
    m_currentPath = experiment->projectPath;

    const QColor statusColor = Status::color(experiment->status);
    m_statusPill->setStyleSheet(pillStyle(statusColor, palette.dark));
    m_statusPill->setText(Status::label(experiment->status));
    m_statusHint->setText(Status::description(experiment->status));

    refreshFavoriteStar(*experiment);

    m_name->setText(experiment->name.isEmpty() ? tr("Untitled") : experiment->name);

    const QString tech = experiment->techDisplay();
    m_tech->setText(tech.isEmpty() ? tr("No stack specified") : tech);

    m_description->setText(experiment->description.trimmed().isEmpty()
                               ? tr("No description yet.")
                               : experiment->description.trimmed());
    m_description->setToolTip(experiment->description);

    m_tags->setTags(experiment->tags);

    m_notes->setText(experiment->notes.trimmed().isEmpty()
                         ? tr("No notes.")
                         : experiment->notes.trimmed());

    if (experiment->projectPath.trimmed().isEmpty()) {
        m_path->setText(tr("No project path"));
        m_path->setToolTip({});
        m_openButton->setEnabled(false);
    } else {
        m_path->setText(experiment->projectPath);
        m_path->setToolTip(experiment->projectPath);
        m_openButton->setEnabled(QFileInfo::exists(experiment->projectPath));
    }

    m_created->setText(tr("Created: %1").arg(formatDateTime(experiment->createdAt)));
    m_modified->setText(tr("Modified: %1").arg(formatDateTime(experiment->lastModified)));

    m_placeholder->setVisible(false);
    m_content->setVisible(true);

    if (auto *contentLayout = m_content->layout())
        contentLayout->invalidate();
}

bool DetailsPanel::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_brackets->parentWidget()
        && event->type() == QEvent::Resize) {
        m_brackets->setGeometry(static_cast<QWidget *>(watched)->rect());
    }
    return QWidget::eventFilter(watched, event);
}
