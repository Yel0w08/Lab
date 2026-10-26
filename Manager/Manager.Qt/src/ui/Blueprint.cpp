#include "ui/Blueprint.h"

#include "core/Theme.h"

#include <QPainter>

namespace {

QColor withAlpha(QColor color, int alpha)
{
    color.setAlpha(alpha);
    return color;
}

void drawCornerBrackets(QPainter &painter, const QRect &area, int inset, int length)
{
    const int x0 = area.left();
    const int y0 = area.top();
    const int x1 = area.right();
    const int y1 = area.bottom();

    // top-left
    painter.drawLine(x0 + inset, y0 + inset, x0 + inset + length, y0 + inset);
    painter.drawLine(x0 + inset, y0 + inset, x0 + inset, y0 + inset + length);
    // top-right
    painter.drawLine(x1 - inset - length, y0 + inset, x1 - inset, y0 + inset);
    painter.drawLine(x1 - inset, y0 + inset, x1 - inset, y0 + inset + length);
    // bottom-left
    painter.drawLine(x0 + inset, y1 - inset - length, x0 + inset, y1 - inset);
    painter.drawLine(x0 + inset, y1 - inset, x0 + inset + length, y1 - inset);
    // bottom-right
    painter.drawLine(x1 - inset, y1 - inset - length, x1 - inset, y1 - inset);
    painter.drawLine(x1 - inset - length, y1 - inset, x1 - inset, y1 - inset);
}

} // namespace

BlueprintPanel::BlueprintPanel(QWidget *parent)
    : QWidget(parent)
{
}

void BlueprintPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const Theme::Palette &p = Theme::current();
    const QRect area = rect();

    QPainter painter(this);
    painter.fillRect(area, QColor(p.window));

    const bool dark = p.dark;
    const QColor minor = withAlpha(QColor(p.border), dark ? 60 : 60);
    const QColor major = withAlpha(QColor(p.borderStrong), dark ? 55 : 70);
    const QColor accent = withAlpha(QColor(p.accent), dark ? 75 : 95);

    const int step = 24;
    const int majorStep = step * 5;

    for (int x = 0; x <= area.right(); x += step) {
        painter.setPen(x % majorStep == 0 ? major : minor);
        painter.drawLine(x, area.top(), x, area.bottom());
    }
    for (int y = 0; y <= area.bottom(); y += step) {
        painter.setPen(y % majorStep == 0 ? major : minor);
        painter.drawLine(area.left(), y, area.right(), y);
    }

    painter.setPen(QPen(accent, 2));
    drawCornerBrackets(painter, area, 10, 14);
}

LabFrame::LabFrame(QWidget *parent)
    : QFrame(parent)
{
}

void LabFrame::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    const Theme::Palette &p = Theme::current();
    const QColor bracket = withAlpha(QColor(p.accent), p.dark ? 150 : 180);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(bracket, 2));
    painter.setBrush(Qt::NoBrush);
    drawCornerBrackets(painter, rect(), 8, 15);
}

BracketOverlay::BracketOverlay(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

void BracketOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const Theme::Palette &p = Theme::current();
    const QColor bracket = withAlpha(QColor(p.accent), p.dark ? 170 : 200);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(bracket, 2));
    painter.setBrush(Qt::NoBrush);
    drawCornerBrackets(painter, rect(), 9, 17);
}