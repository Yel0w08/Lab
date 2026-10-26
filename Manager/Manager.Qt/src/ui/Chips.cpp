#include "ui/Chips.h"

#include <QFontMetrics>
#include <QPainterPath>

namespace Chips {
namespace {

QStringList darkColors()
{
    return {
        QStringLiteral("#33D6E8"),
        QStringLiteral("#3DDC97"),
        QStringLiteral("#A78BFA"),
        QStringLiteral("#FFB648"),
        QStringLiteral("#FF7EA6"),
        QStringLiteral("#5B8DEF"),
    };
}

QStringList lightColors()
{
    return {
        QStringLiteral("#0B84A0"),
        QStringLiteral("#0E8F5F"),
        QStringLiteral("#6D4FE0"),
        QStringLiteral("#C97A00"),
        QStringLiteral("#C2255C"),
        QStringLiteral("#1D5BD1"),
    };
}

uint stableHash(const QString &tag)
{
    uint hash = 2166136261u;
    for (const QChar c : tag) {
        hash ^= c.unicode();
        hash *= 16777619u;
    }
    return hash;
}

int chipWidth(const QString &tag, const QFontMetrics &fm, int padding)
{
    return fm.horizontalAdvance(tag) + padding * 2;
}

void paintChip(QPainter &painter, const QString &tag, const QRect &rect,
               const QFont &font, bool dark, int padding)
{
    painter.save();

    const QColor base = colorFor(tag, dark);
    QColor background = base;
    background.setAlpha(dark ? 44 : 26);

    QColor border = base;
    border.setAlpha(dark ? 110 : 80);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(background);
    painter.drawRect(rect);

    painter.setPen(QPen(border, 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect.adjusted(1, 1, -1, -1));

    QColor text = base;
    if (dark)
        text = text.lighter(112);
    else
        text = text.darker(108);

    painter.setFont(font);
    painter.setPen(text);
    painter.drawText(rect.adjusted(padding, 0, -padding, 0),
                     Qt::AlignLeft | Qt::AlignVCenter, tag);

    painter.restore();
}

} // namespace

QColor colorFor(const QString &tag, bool dark)
{
    const QStringList palette = dark ? darkColors() : lightColors();
    return QColor(palette.at(int(stableHash(tag) % uint(palette.size()))));
}

int rowWidth(const QStringList &tags, const QFont &font, int gap, int padding)
{
    const QFontMetrics fm(font);
    int width = 0;
    bool first = true;
    for (const QString &tag : tags) {
        if (!first)
            width += gap;
        width += chipWidth(tag, fm, padding);
        first = false;
    }
    return width;
}

void paintRow(QPainter &painter, const QStringList &tags, const QRect &rect,
              const QFont &font, bool dark, int gap, int chipHeight, int padding)
{
    if (tags.isEmpty() || rect.width() <= 0)
        return;

    const QFontMetrics fm(font);
    const int y = rect.center().y() - chipHeight / 2;
    int x = rect.left();

    int painted = 0;
    for (const QString &tag : tags) {
        const int width = chipWidth(tag, fm, padding);
        if (x + width > rect.right()) {
            const int remaining = tags.size() - painted;
            if (remaining > 0) {
                const QString more = QStringLiteral("+%1").arg(remaining);
                const QRect badge(x + 2, y, fm.horizontalAdvance(more) + padding, chipHeight);
                if (badge.right() <= rect.right()) {
                    painter.save();
                    painter.setFont(font);
                    painter.setPen(QColor(dark ? QStringLiteral("#5E6A7B")
                                               : QStringLiteral("#97A3B4")));
                    painter.drawText(badge, Qt::AlignCenter, more);
                    painter.restore();
                }
            }
            return;
        }

        paintChip(painter, tag, QRect(x, y, width, chipHeight), font, dark, padding);
        x += width + gap;
        ++painted;
    }
}

int heightForWidth(const QStringList &tags, const QFont &font, int width,
                   int gap, int chipHeight, int padding)
{
    if (tags.isEmpty() || width <= 0)
        return 0;

    const QFontMetrics fm(font);
    int x = 0;
    int rows = 1;

    for (const QString &tag : tags) {
        const int w = chipWidth(tag, fm, padding);
        if (x > 0 && x + w > width) {
            ++rows;
            x = 0;
        }
        x += w + gap;
    }
    return rows * chipHeight + (rows - 1) * gap;
}

void paintWrapped(QPainter &painter, const QStringList &tags, const QRect &rect,
                  const QFont &font, bool dark, int gap, int chipHeight, int padding)
{
    if (tags.isEmpty() || rect.width() <= 0)
        return;

    const QFontMetrics fm(font);
    int x = rect.left();
    int y = rect.top();

    for (const QString &tag : tags) {
        const int w = chipWidth(tag, fm, padding);
        if (x > rect.left() && x + w > rect.right()) {
            x = rect.left();
            y += chipHeight + gap;
        }
        paintChip(painter, tag, QRect(x, y, w, chipHeight), font, dark, padding);
        x += w + gap;
    }
}

} // namespace Chips
