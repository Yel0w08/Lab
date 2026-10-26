#pragma once

#include <QFont>
#include <QString>

class QApplication;

/**
 * Central styling for the app: a colour palette + a generated style sheet.
 * `Theme::current()` is used by every custom painted widget (nav rail, tags,
 * status pills, ...) so switching the theme repaints everything consistently.
 */
namespace Theme {

struct Palette {
    bool dark = true;

    QString font;
    QString mono;
    int fontPx = 13;

    QString window;
    QString sidebar;
    QString surface;
    QString surfaceAlt;
    QString surfaceAlt2;
    QString surfaceHover;
    QString cardTop;

    QString border;
    QString borderStrong;

    QString text;
    QString textDim;
    QString textFaint;

    QString accent;
    QString accent2;
    QString accentHover;
    QString accent2Hover;
    QString accentPress;
    QString accentText;
    QString accentSoft;
    QString accentSel;

    QString danger;
    QString dangerBg;
    QString dangerBorder;

    QString star;
    QString rowHover;
    QString rowBorder;

    QString inputBg;
    QString inputFocus;

    QString scrollHandle;
    QString scrollHandleHover;
};

const Palette &current();
bool isDark();

Palette darkPalette();
Palette lightPalette();

QString styleSheet(const Palette &palette);

/** Applies the palette + generated style sheet to the application. */
void apply(QApplication &app, bool dark);
void toggle(QApplication &app);

/**
 * Returns `font` resized by `delta` pixels. Styles sheets size fonts in
 * pixels, in which case `pointSizeF()` is invalid, so both cases are handled.
 */
QFont scaledFont(const QFont &font, int delta);

QString preferredFontFamily();
QString preferredMonoFontFamily();

/**
 * Same glyph sizes as `tmpl` but switched to the technical monospace family
 * used for labels, tags, statuses, spec lines, etc.
 */
QFont monoFont(const QFont &tmpl);

} // namespace Theme
