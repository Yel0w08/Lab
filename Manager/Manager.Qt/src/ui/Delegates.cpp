#include "ui/Delegates.h"

#include "core/Theme.h"
#include "models/ExperimentTableModel.h"
#include "ui/Chips.h"

#include <QPainter>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QSortFilterProxyModel>

namespace {

const Experiment *experimentAt(const QModelIndex &index)
{
    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(index.model());

    if (auto *source = qobject_cast<ExperimentTableModel *>(model))
        return &source->experiment(index.row());

    if (auto *proxy = qobject_cast<QSortFilterProxyModel *>(model)) {
        auto *source = qobject_cast<ExperimentTableModel *>(proxy->sourceModel());
        const QModelIndex sourceIndex = proxy->mapToSource(index);
        if (source && sourceIndex.isValid())
            return &source->experiment(sourceIndex.row());
    }
    return nullptr;
}

bool isHovered(const ExperimentTableModel *model, const QModelIndex &index)
{
    return model && model->hoverRow() == index.row();
}

} // namespace

BaseDelegate::BaseDelegate(const ExperimentTableModel *model, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(model)
{
}

QSize BaseDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(60, 46);
}

void BaseDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    painter->save();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const Theme::Palette &palette = Theme::current();

    // Fill the current cell only. Cells are painted left-to-right and tile
    // exactly, so each cell fill is seamless with its neighbours while never
    // overpainting content that an earlier cell painted.
    const QRect cellRect = opt.rect;

    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(cellRect, QColor(palette.accentSel));
    } else if (isHovered(m_model, index)) {
        painter->fillRect(cellRect, QColor(palette.rowHover));
    }

    painter->fillRect(QRect(cellRect.left(), cellRect.bottom(), cellRect.width(), 1),
                      QColor(palette.rowBorder));

    paintContent(painter, opt, index);
    painter->restore();
}

void BaseDelegate::paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    drawPlain(painter, option, index);
}

void BaseDelegate::drawPlain(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    const Theme::Palette &palette = Theme::current();
    QRect rect = option.rect.adjusted(12, 0, -10, 0);
    drawText(painter, option, rect, index.data(Qt::DisplayRole).toString(),
             Theme::monoFont(option.font), QColor(palette.textDim));
}

void BaseDelegate::drawText(QPainter *painter, const QStyleOptionViewItem &option,
                            const QRect &rect, const QString &text, const QFont &font,
                            const QColor &color) const
{
    Q_UNUSED(option)

    if (text.isEmpty() || rect.width() <= 0)
        return;

    painter->setFont(font);
    painter->setPen(color);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter,
                      QFontMetrics(font).elidedText(text, Qt::ElideRight, rect.width()));
}

// ---------------------------------------------------------------- StarDelegate

StarDelegate::StarDelegate(const ExperimentTableModel *model, QObject *parent)
    : BaseDelegate(model, parent)
{
}

void StarDelegate::paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    const Theme::Palette &palette = Theme::current();
    const bool favorite = index.data(ExperimentTableModel::FavoriteRole).toBool();
    const bool hovered = isHovered(m_model, index);

    QFont font = Theme::scaledFont(option.font, 7);
    font.setBold(true);

    QColor color;
    if (favorite)
        color = QColor(palette.star);
    else if (hovered)
        color = QColor(palette.textDim);
    else
        color = QColor(palette.textFaint);

    painter->setFont(font);
    painter->setPen(color);
    painter->drawText(option.rect, Qt::AlignCenter,
                      favorite ? QStringLiteral("★") : QStringLiteral("☆"));
}

// ------------------------------------------------------------ StatusDelegate

StatusDelegate::StatusDelegate(const ExperimentTableModel *model, QObject *parent)
    : BaseDelegate(model, parent)
{
}

void StatusDelegate::paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    const Experiment *e = experimentAt(index);
    if (!e)
        return;

    const Theme::Palette &palette = Theme::current();
    const QColor base = Status::color(e->status);

    QFont font = Theme::monoFont(option.font);
    font.setWeight(QFont::DemiBold);

    const QString text = Status::label(e->status);
    const QFontMetrics fm(font);

    const int dotDiameter = 8;
    const int pillHeight = 24;
    const int leftPad = 11;
    const int gap = 8;
    const int rightPad = 12;

    const int pillWidth = leftPad + dotDiameter + gap + fm.horizontalAdvance(text) + rightPad;
    const QRect pill(option.rect.left() + 12,
                     option.rect.center().y() - pillHeight / 2,
                     pillWidth,
                     pillHeight);

    QColor background = base;
    background.setAlpha(palette.dark ? 42 : 30);

    QColor border = base;
    border.setAlpha(palette.dark ? 96 : 70);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(background);
    painter->drawRect(pill);

    painter->setPen(QPen(border, 1.0));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(pill.adjusted(1, 1, -1, -1));

    painter->setPen(Qt::NoPen);
    painter->setBrush(base);

    // Diamond marker (geometric) instead of a soft circle.
    painter->save();
    painter->translate(pill.left() + leftPad + dotDiameter / 2.0,
                       pill.center().y());
    painter->rotate(45.0);
    painter->drawRect(QRectF(-3.5, -3.5, 7.0, 7.0));
    painter->restore();

    const QRect textRect(pill.left() + leftPad + dotDiameter + gap,
                         pill.top(),
                         fm.horizontalAdvance(text) + 2,
                         pill.height());

    painter->setFont(font);
    painter->setPen(base);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

// ------------------------------------------------------------- NameDelegate

NameDelegate::NameDelegate(const ExperimentTableModel *model, QObject *parent)
    : BaseDelegate(model, parent)
{
}

void NameDelegate::paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    const Experiment *e = experimentAt(index);
    if (!e)
        return;

    const Theme::Palette &palette = Theme::current();

    QFont nameFont = option.font;
    nameFont.setWeight(QFont::DemiBold);

    const QFont subFont = Theme::scaledFont(option.font, -2);

    const QRect nameRect(option.rect.left() + 12, option.rect.top() + 7,
                         option.rect.width() - 24, 18);
    const QRect subRect(option.rect.left() + 12, option.rect.top() + 25,
                        option.rect.width() - 24, 14);

    drawText(painter, option, nameRect, e->name, nameFont, QColor(palette.text));

    QString subtitle = e->description.trimmed();
    if (subtitle.isEmpty())
        subtitle = e->techDisplay();
    if (subtitle.isEmpty())
        subtitle = tr("No description");

    drawText(painter, option, subRect, subtitle, subFont, QColor(palette.textDim));
}

// ------------------------------------------------------------- TagsDelegate

TagsDelegate::TagsDelegate(const ExperimentTableModel *model, QObject *parent)
    : BaseDelegate(model, parent)
{
}

void TagsDelegate::paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    const QStringList tags = index.data(ExperimentTableModel::TagsRole).toStringList();
    if (tags.isEmpty())
        return;

    const QFont font = Theme::monoFont(Theme::scaledFont(option.font, -2));

    const QRect rect = option.rect.adjusted(12, 0, -6, 0);
    Chips::paintRow(*painter, tags, rect, font, Theme::current().dark, 6, 22, 9);
}

// ---------------------------------------------------------- CheckDelegate

CheckDelegate::CheckDelegate(const ExperimentTableModel *model, QObject *parent)
    : BaseDelegate(model, parent)
{
}

void CheckDelegate::paintContent(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    const Theme::Palette &palette = Theme::current();
    const bool checked = index.data(ExperimentTableModel::DownloadableRole).toBool();

    const int boxSize = 17;
    const QRect box(option.rect.left() + 14,
                    option.rect.center().y() - boxSize / 2,
                    boxSize,
                    boxSize);

    painter->setRenderHint(QPainter::Antialiasing, true);

    if (checked) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(palette.accent));
        painter->drawRect(box);

        QPainterPath path;
        path.moveTo(box.left() + 4.5, box.top() + 8.5);
        path.lineTo(box.left() + 7.5, box.top() + 11.5);
        path.lineTo(box.left() + 12.5, box.top() + 5.5);

        painter->setPen(QPen(QColor(palette.accentText), 1.8, Qt::SolidLine,
                             Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path);
    } else {
        painter->setPen(QPen(QColor(palette.borderStrong), 1.2));
        painter->setBrush(isHovered(m_model, index) ? QColor(palette.surfaceAlt)
                                                    : QColor(palette.inputBg));
        painter->drawRect(box);
    }
}
