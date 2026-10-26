#include "core/Theme.h"

#include <QApplication>
#include <QFontDatabase>

namespace Theme {
namespace {

Palette g_palette;
bool g_initialised = false;

QString expand(QString sheet, const Palette &p)
{
    const struct { const char *key; const QString *value; } tokens[] = {
        {"%{font}",            &p.font},
        {"%{mono}",            &p.mono},
        {"%{fontPx}",          nullptr},
        {"%{window}",          &p.window},
        {"%{sidebar}",         &p.sidebar},
        {"%{surface}",         &p.surface},
        {"%{surfaceAlt}",      &p.surfaceAlt},
        {"%{surfaceAlt2}",     &p.surfaceAlt2},
        {"%{surfaceHover}",    &p.surfaceHover},
        {"%{cardTop}",         &p.cardTop},
        {"%{border}",          &p.border},
        {"%{borderStrong}",    &p.borderStrong},
        {"%{text}",            &p.text},
        {"%{textDim}",         &p.textDim},
        {"%{textFaint}",       &p.textFaint},
        {"%{accent}",          &p.accent},
        {"%{accent2}",         &p.accent2},
        {"%{accentHover}",     &p.accentHover},
        {"%{accent2Hover}",    &p.accent2Hover},
        {"%{accentPress}",     &p.accentPress},
        {"%{accentText}",      &p.accentText},
        {"%{accentSoft}",      &p.accentSoft},
        {"%{accentSel}",       &p.accentSel},
        {"%{danger}",          &p.danger},
        {"%{dangerBg}",        &p.dangerBg},
        {"%{dangerBorder}",    &p.dangerBorder},
        {"%{star}",            &p.star},
        {"%{rowHover}",        &p.rowHover},
        {"%{rowBorder}",       &p.rowBorder},
        {"%{inputBg}",         &p.inputBg},
        {"%{inputFocus}",      &p.inputFocus},
        {"%{scrollHandle}",    &p.scrollHandle},
        {"%{scrollHandleHover}", &p.scrollHandleHover},
    };

    for (const auto &token : tokens) {
        if (token.value)
            sheet.replace(QLatin1String(token.key), *token.value);
        else
            sheet.replace(QLatin1String(token.key), QString::number(p.fontPx));
    }
    return sheet;
}

} // namespace

QFont scaledFont(const QFont &font, int delta)
{
    QFont result = font;
    if (font.pixelSize() > 0)
        result.setPixelSize(qMax(1, font.pixelSize() + delta));
    else
        result.setPointSizeF(qMax(1.0, font.pointSizeF() + delta));
    return result;
}

QString preferredFontFamily()
{
    const QStringList candidates = {
        QStringLiteral("Inter"),
        QStringLiteral("Segoe UI"),
        QStringLiteral("SF Pro Text"),
        QStringLiteral("Cantarell"),
        QStringLiteral("Adwaita Sans"),
        QStringLiteral("Noto Sans"),
        QStringLiteral("DejaVu Sans"),
        QStringLiteral("Ubuntu"),
        QStringLiteral("Helvetica Neue"),
        QStringLiteral("Arial"),
    };

    const QStringList installed = QFontDatabase::families();
    for (const QString &candidate : candidates) {
        if (installed.contains(candidate, Qt::CaseInsensitive))
            return candidate;
    }
    return QGuiApplication::font().family();
}

QString preferredMonoFontFamily()
{
    const QStringList candidates = {
        QStringLiteral("JetBrains Mono"),
        QStringLiteral("Fira Code"),
        QStringLiteral("Cascadia Code"),
        QStringLiteral("IBM Plex Mono"),
        QStringLiteral("Source Code Pro"),
        QStringLiteral("Menlo"),
        QStringLiteral("Consolas"),
        QStringLiteral("DejaVu Sans Mono"),
        QStringLiteral("Ubuntu Mono"),
        QStringLiteral("Liberation Mono"),
        QStringLiteral("Monospace"),
    };

    const QStringList installed = QFontDatabase::families();
    for (const QString &candidate : candidates) {
        if (installed.contains(candidate, Qt::CaseInsensitive))
            return candidate;
    }
    return preferredFontFamily();
}

QFont monoFont(const QFont &tmpl)
{
    QFont result = tmpl;
    result.setFamily(preferredMonoFontFamily());
    return result;
}

Palette darkPalette()
{
    Palette p;
    p.dark = true;
    p.font = preferredFontFamily();
    p.mono = preferredMonoFontFamily();
    p.fontPx = 13;

    // Deep-space navy, electric cyan + violet — a computation lab instrument.
    p.window       = QStringLiteral("#070B12");
    p.sidebar      = QStringLiteral("#0A1018");
    p.surface      = QStringLiteral("#0D1420");
    p.surfaceAlt   = QStringLiteral("#131E2C");
    p.surfaceAlt2  = QStringLiteral("#1B2A3C");
    p.surfaceHover = QStringLiteral("#15202F");
    p.cardTop      = QStringLiteral("#111C2B");

    p.border       = QStringLiteral("#1E2B3D");
    p.borderStrong = QStringLiteral("#2F4259");

    p.text         = QStringLiteral("#D9E6F6");
    p.textDim      = QStringLiteral("#8A9FBA");
    p.textFaint    = QStringLiteral("#54698A");

    p.accent       = QStringLiteral("#33D6E8");
    p.accent2      = QStringLiteral("#6C6CF5");
    p.accentHover  = QStringLiteral("#4FE3F2");
    p.accent2Hover = QStringLiteral("#8585FA");
    p.accentPress  = QStringLiteral("#22B7CC");
    p.accentText   = QStringLiteral("#03141B");
    p.accentSoft   = QStringLiteral("#0F3142");
    p.accentSel    = QStringLiteral("#12384A");

    p.danger       = QStringLiteral("#FF4D6D");
    p.dangerBg     = QStringLiteral("#2A0F16");
    p.dangerBorder = QStringLiteral("#4B1F2C");

    p.star         = QStringLiteral("#FFC857");
    p.rowHover     = QStringLiteral("#111B2A");
    p.rowBorder    = QStringLiteral("#152033");

    p.inputBg      = QStringLiteral("#0A1019");
    p.inputFocus   = QStringLiteral("#0D1520");

    p.scrollHandle    = QStringLiteral("#223248");
    p.scrollHandleHover = QStringLiteral("#33496B");

    return p;
}

Palette lightPalette()
{
    Palette p;
    p.dark = false;
    p.font = preferredFontFamily();
    p.mono = preferredMonoFontFamily();
    p.fontPx = 13;

    // "Lab notebook" paper blueprint: near-white, cyan + violet accents.
    p.window       = QStringLiteral("#EAF0F8");
    p.sidebar      = QStringLiteral("#F4F8FC");
    p.surface      = QStringLiteral("#FFFFFF");
    p.surfaceAlt   = QStringLiteral("#F0F5FB");
    p.surfaceAlt2  = QStringLiteral("#E1EAF4");
    p.surfaceHover = QStringLiteral("#EAF1FA");
    p.cardTop      = QStringLiteral("#FFFFFF");

    p.border       = QStringLiteral("#D2DDEA");
    p.borderStrong = QStringLiteral("#AFC4DA");

    p.text         = QStringLiteral("#16222F");
    p.textDim      = QStringLiteral("#4A5F79");
    p.textFaint    = QStringLiteral("#8AA0BA");

    p.accent       = QStringLiteral("#0E8FA6");
    p.accent2      = QStringLiteral("#5A4FCF");
    p.accentHover  = QStringLiteral("#0AA7C2");
    p.accent2Hover = QStringLiteral("#6A5FE0");
    p.accentPress  = QStringLiteral("#0B7A8F");
    p.accentText   = QStringLiteral("#FFFFFF");
    p.accentSoft   = QStringLiteral("#D9F2F8");
    p.accentSel    = QStringLiteral("#C6EDF5");

    p.danger       = QStringLiteral("#D92E50");
    p.dangerBg     = QStringLiteral("#FCEDF0");
    p.dangerBorder = QStringLiteral("#F4C4CE");

    p.star         = QStringLiteral("#D98E04");
    p.rowHover     = QStringLiteral("#F2F7FC");
    p.rowBorder    = QStringLiteral("#E8EFF7");

    p.inputBg      = QStringLiteral("#FFFFFF");
    p.inputFocus   = QStringLiteral("#FFFFFF");

    p.scrollHandle    = QStringLiteral("#CBD9E8");
    p.scrollHandleHover = QStringLiteral("#AABFD8");

    return p;
}

const Palette &current()
{
    if (!g_initialised) {
        g_palette = darkPalette();
        g_initialised = true;
    }
    return g_palette;
}

bool isDark()
{
    return current().dark;
}

QString styleSheet(const Palette &palette)
{
    static const char *sheet = R"QSS(
* {
    font-family: "%{font}";
    font-size: %{fontPx}px;
    outline: 0;
}

QMainWindow, QDialog {
    background: %{window};
}

QWidget#sidebar { background: %{sidebar}; border-right: 1px solid %{border}; }

QLabel { background: transparent; color: %{text}; }

QLabel#appMark {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 %{accent}, stop:1 %{accent2});
    color: %{accentText};
    border-radius: 0px;
    font-size: 16px;
    font-weight: 800;
}

QLabel#appTitle      { font-size: 17px; font-weight: 700; letter-spacing: 0.4px; }
QLabel#appSub        { font-family: "%{mono}"; font-size: 11px; color: %{textFaint};
                       font-weight: 600; letter-spacing: 0.8px; }
QLabel#sectionLabel  { font-family: "%{mono}"; font-size: 10px; font-weight: 700;
                       color: %{textFaint}; letter-spacing: 1.4px; padding: 0px 12px; }
QLabel#cardSection   { font-family: "%{mono}"; font-size: 10px; font-weight: 700;
                       color: %{textFaint}; letter-spacing: 1.2px; padding: 0px; }
QLabel#detailsStar   { font-size: 20px; font-weight: 700; }
QLabel#pageTitle     { font-size: 22px; font-weight: 700; }
QLabel#pageSub       { font-family: "%{mono}"; font-size: 11px; color: %{textDim}; }
QLabel#hint          { font-family: "%{mono}"; font-size: 11px; color: %{textFaint}; }
QLabel#fieldLabel    { font-size: 11px; font-weight: 700; color: %{textDim}; padding: 0px; }
QLabel#muted         { color: %{textDim}; }
QLabel#countBadge    {
    background: %{surfaceAlt};
    border: 1px solid %{border};
    border-radius: 2px;
    color: %{textDim};
    padding: 2px 9px;
    font-family: "%{mono}";
    font-size: 11px;
    font-weight: 700;
}
QLabel#statValue     { font-family: "%{mono}"; font-size: 20px; font-weight: 700; }
QLabel#statTitle     { font-family: "%{mono}"; font-size: 11px; color: %{textDim};
                       font-weight: 600; }

QFrame#sidebar       { background: %{sidebar}; border-right: 1px solid %{border}; }
QFrame#card {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 %{cardTop}, stop:1 %{surface});
    border: 1px solid %{border};
    border-top-color: %{borderStrong};
    border-left-color: %{borderStrong};
    border-right-color: %{border};
    border-bottom-color: %{window};
    border-radius: 0px;
}
QFrame#emptyState    { background: transparent; border: none; }
QFrame#separator     { background: %{border}; min-height: 1px; max-height: 1px; border: none; }

QScrollArea#detailsScroll { background: transparent; border: none; }
QScrollArea#detailsScroll > QWidget > QWidget { background: transparent; border: none; }

QPushButton#navItem {
    background: transparent;
    border: none;
    border-radius: 0px;
    padding: 0px;
}
QPushButton#navItem:checked { background: transparent; }

QLabel#overlayTitle  { font-size: 17px; font-weight: 700; color: %{textDim}; }
QLabel#overlayText   { font-size: 12px; color: %{textFaint}; }
QLabel#overlayGlyph  { font-size: 34px; color: %{borderStrong}; }

QPushButton {
    background: %{surfaceAlt};
    border: 1px solid %{border};
    border-radius: 0px;
    padding: 7px 14px;
    color: %{text};
    font-weight: 600;
}
QPushButton:hover      { background: %{surfaceHover}; border-color: %{borderStrong}; }
QPushButton:pressed    { background: %{surfaceAlt2}; }
QPushButton:disabled   { background: %{surface}; border-color: %{border}; color: %{textFaint}; }

QPushButton#primaryButton {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 %{accent}, stop:1 %{accent2});
    border: 1px solid %{accent};
    color: %{accentText};
    padding: 8px 16px;
}
QPushButton#primaryButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 %{accentHover}, stop:1 %{accent2Hover});
    border-color: %{accentHover};
}
QPushButton#primaryButton:pressed   { background: %{accentPress}; border-color: %{accentPress}; }
QPushButton#primaryButton:disabled  { background: %{surfaceAlt}; border-color: %{border}; color: %{textFaint}; }

QPushButton#dangerButton {
    background: transparent;
    border: 1px solid %{dangerBorder};
    color: %{danger};
}
QPushButton#dangerButton:hover   { background: %{dangerBg}; }
QPushButton#dangerButton:pressed { background: %{dangerBg}; }

QPushButton#softButton {
    background: transparent;
    border: 1px solid transparent;
    color: %{textDim};
}
QPushButton#softButton:hover  { background: %{surfaceHover}; border-color: %{border}; color: %{text}; }
QPushButton#softButton:checked {
    background: %{accentSoft};
    border-color: %{accent};
    color: %{accent};
}

QLineEdit, QPlainTextEdit, QComboBox {
    background: %{inputBg};
    border: 1px solid %{border};
    border-radius: 2px;
    padding: 7px 10px;
    color: %{text};
    selection-background-color: %{accent};
    selection-color: %{accentText};
}
QLineEdit:focus, QPlainTextEdit:focus, QComboBox:focus {
    border: 1px solid %{accent};
    background: %{inputFocus};
}
QLineEdit#searchEdit {
    padding: 8px 12px;
    background: %{surface};
    border: 1px solid %{borderStrong};
    border-radius: 0px;
    font-family: "%{mono}";
    font-size: 12px;
}
QLineEdit#searchEdit:focus { background: %{inputFocus}; border-color: %{accent}; }
QLineEdit#invalid {
    border: 1px solid %{danger};
    background: %{dangerBg};
}
QPlainTextEdit { font-size: 12px; }

QComboBox { padding: 7px 26px 7px 10px; }
QComboBox::drop-down {
    border: none;
    width: 26px;
    background: transparent;
}
QComboBox::down-arrow {
    image: none;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid %{textDim};
    margin-right: 10px;
}
QComboBox QAbstractItemView {
    background: %{surface};
    border: 1px solid %{border};
    border-radius: 2px;
    padding: 5px;
    selection-background-color: %{accentSoft};
    selection-color: %{text};
    outline: 0;
}
QComboBox QAbstractItemView::item { padding: 6px 8px; border-radius: 2px; min-height: 20px; }

QCheckBox { spacing: 9px; color: %{text}; font-weight: 600; }
QCheckBox::indicator {
    width: 18px;
    height: 18px;
    border-radius: 2px;
    border: 1px solid %{borderStrong};
    background: %{inputBg};
}
QCheckBox::indicator:hover { border-color: %{accent}; }
QCheckBox::indicator:checked {
    background: %{accent};
    border-color: %{accent};
}

QTableView {
    background: %{surface};
    alternate-background-color: %{surface};
    border: none;
    border-radius: 0px;
    gridline-color: transparent;
    selection-background-color: transparent;
    selection-color: %{text};
    outline: 0;
}
QTableView::item { background: transparent; border: none; padding: 0px; }
QTableView::item:selected { background: transparent; color: %{text}; }

QHeaderView::section {
    background: %{surface};
    color: %{textFaint};
    border: none;
    border-bottom: 1px solid %{border};
    padding: 9px 10px;
    font-family: "%{mono}";
    font-size: 10px;
    font-weight: 700;
    letter-spacing: 0.6px;
}
QHeaderView::section:hover { color: %{textDim}; }
QHeaderView::up-arrow,
QHeaderView::down-arrow { image: none; width: 0px; height: 0px; }

QAbstractScrollArea::corner { background: transparent; }
QScrollBar:vertical {
    background: transparent;
    width: 12px;
    margin: 3px;
}
QScrollBar::handle:vertical {
    background: %{scrollHandle};
    border-radius: 0px;
    min-height: 34px;
}
QScrollBar::handle:vertical:hover { background: %{scrollHandleHover}; }
QScrollBar:horizontal {
    background: transparent;
    height: 12px;
    margin: 3px;
}
QScrollBar::handle:horizontal {
    background: %{scrollHandle};
    border-radius: 0px;
    min-width: 34px;
}
QScrollBar::handle:horizontal:hover { background: %{scrollHandleHover}; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0px; height: 0px; }
QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

QToolTip {
    background: %{surfaceAlt2};
    color: %{text};
    border: 1px solid %{borderStrong};
    border-radius: 2px;
    padding: 5px 8px;
    font-size: 11px;
}

QMenu, QMenuBar {
    background: %{surface};
    border: 1px solid %{border};
    border-radius: 2px;
    padding: 6px;
    color: %{text};
}
QMenu::item {
    background: transparent;
    border-radius: 0px;
    padding: 7px 26px 7px 12px;
}
QMenu::item:selected { background: %{accentSoft}; color: %{text}; }
QMenu::item:disabled { color: %{textFaint}; }
QMenu::separator { height: 1px; background: %{border}; margin: 5px 8px; }
QMenu::indicator { width: 14px; height: 14px; }

QMessageBox { background: %{surface}; }
QMessageBox QLabel { color: %{text}; font-size: 13px; }

QStatusBar { background: transparent; color: %{textFaint}; }

QSplitter::handle { background: transparent; }
)QSS";

    return expand(QLatin1String(sheet), palette);
}

void apply(QApplication &app, bool dark)
{
    g_palette = dark ? darkPalette() : lightPalette();
    g_initialised = true;

    app.setFont(QFont(g_palette.font, g_palette.fontPx));
    app.setStyleSheet(styleSheet(g_palette));
}

void toggle(QApplication &app)
{
    apply(app, !isDark());
}

} // namespace Theme
