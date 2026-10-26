#pragma once

#include <QColor>

// YLED theme — sharp, square, pro black / white with a restrained yellow accent.
namespace Theme {

inline const QColor Background   (0x00, 0x00, 0x00);
inline const QColor Panel        (0x0A, 0x0A, 0x0A);
inline const QColor PanelAlt     (0x05, 0x05, 0x05);
inline const QColor Border       (0x2A, 0x2A, 0x2A);
inline const QColor BorderBright (0x55, 0x55, 0x55);
inline const QColor Text         (0xFF, 0xFF, 0xFF);
inline const QColor Dim          (0x9A, 0x9A, 0x9A);
inline const QColor Yellow       (0xFF, 0xD4, 0x00);
inline const QColor YellowBright (0xFF, 0xE5, 0x4D);
inline const QColor Danger       (0xFF, 0x3B, 0x3B);
inline const QColor Success      (0x2E, 0xD5, 0x73);
inline const QColor Accent       (0xFF, 0xD4, 0x00);

} // namespace Theme
