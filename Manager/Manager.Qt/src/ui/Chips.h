#pragma once

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QRect>
#include <QStringList>

/**
 * Small helper used by both the table delegate and the details panel to paint
 * tags as coloured chips.
 */
namespace Chips {

/** Stable, pleasant colour for a tag (does not change between runs). */
QColor colorFor(const QString &tag, bool dark);

/** Width needed to paint every tag on one row. */
int rowWidth(const QStringList &tags, const QFont &font, int gap = 6, int padding = 9);

/** Paints one row of chips, clipping the overflow and showing a "+n" badge. */
void paintRow(QPainter &painter, const QStringList &tags, const QRect &rect,
              const QFont &font, bool dark, int gap = 6, int chipHeight = 22,
              int padding = 9);

/** Height needed to paint the tags wrapped at the given width. */
int heightForWidth(const QStringList &tags, const QFont &font, int width,
                   int gap = 6, int chipHeight = 22, int padding = 9);

/** Paints the tags wrapped on as many rows as needed. */
void paintWrapped(QPainter &painter, const QStringList &tags, const QRect &rect,
                  const QFont &font, bool dark, int gap = 6, int chipHeight = 22,
                  int padding = 9);

} // namespace Chips
