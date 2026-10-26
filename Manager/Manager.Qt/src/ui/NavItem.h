#pragma once

#include "core/Theme.h"

#include <QEnterEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QPushButton>
#include <QFontMetrics>

/**
 * Custom painted entry of the left navigation rail: colour dot, label and a
 * count badge, with an accent bar when active.
 */
class NavItem : public QPushButton
{
public:
    NavItem(const QString &label, const QColor &dotColor, QWidget *parent = nullptr)
        : QPushButton(parent)
        , m_label(label)
        , m_dot(dotColor)
    {
        setObjectName(QStringLiteral("navItem"));
        setCursor(Qt::PointingHandCursor);
        setCheckable(true);
        setAutoExclusive(true);
        setFixedHeight(38);
        setFocusPolicy(Qt::NoFocus);
    }

    void setCount(int count)
    {
        if (m_count == count)
            return;
        m_count = count;
        update();
    }

    void setDotColor(const QColor &color)
    {
        m_dot = color;
        update();
    }

    QString itemLabel() const { return m_label; }

    QSize sizeHint() const override { return QSize(204, 38); }
    QSize minimumSizeHint() const override { return QSize(160, 38); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        const Theme::Palette &p = Theme::current();

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRect r = rect().adjusted(8, 4, -8, -4);
        const bool active = isChecked();

        QColor background;
        if (active)
            background = QColor(p.accentSoft);
        else if (m_hover)
            background = QColor(p.surfaceHover);

        if (background.isValid()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(background);
            painter.drawRect(r);
        }

        if (active) {
            QRect bar(r.left() + 4, r.center().y() - 9, 3, 18);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(p.accent));
            painter.drawRect(bar);
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(m_dot);

        // Geometric diamond indicator instead of a soft circle.
        painter.save();
        painter.translate(QPointF(r.left() + 26, r.center().y()));
        painter.rotate(45.0);
        painter.drawRect(QRectF(-4.0, -4.0, 8.0, 8.0));
        painter.restore();

        QFont font = Theme::monoFont(this->font());
        font.setWeight(active ? QFont::DemiBold : QFont::Medium);
        painter.setFont(font);
        painter.setPen(QColor(active ? p.text : p.textDim));

        const QRect labelRect(r.left() + 40, r.top(), r.width() - 40 - 40, r.height());
        painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetrics(font).elidedText(m_label, Qt::ElideRight,
                                                       labelRect.width()));

        if (m_count > 0) {
            painter.setPen(QColor(active ? p.accent : p.textFaint));
            const QRect countRect(r.right() - 36, r.top(), 30, r.height());
            painter.drawText(countRect, Qt::AlignRight | Qt::AlignVCenter,
                             QString::number(m_count));
        }
    }

    void enterEvent(QEnterEvent *event) override
    {
        m_hover = true;
        update();
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        m_hover = false;
        update();
        QPushButton::leaveEvent(event);
    }

private:
    QString m_label;
    QColor m_dot;
    int m_count = 0;
    bool m_hover = false;
};
