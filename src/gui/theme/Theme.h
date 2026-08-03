#pragma once

#include <QString>

namespace Gui::Theme {

// Palette and typography from the compact OpenATC desktop design.
inline constexpr const char *kBg = "#191919";
inline constexpr const char *kBgRaised = "#1E1E1E";
inline constexpr const char *kBgSidebar = "#202020";
inline constexpr const char *kBgButton = "#353535";
inline constexpr const char *kBgButtonHover = "#414141";
inline constexpr const char *kBgCard = "#262626";
inline constexpr const char *kBorder = "#2B2B2B";
inline constexpr const char *kText = "#F1F1F1";
inline constexpr const char *kTextMuted = "#BBBBBB";
inline constexpr const char *kTextDim = "#858585";
inline constexpr const char *kAccent = "#38D75F";
inline constexpr const char *kAccentDim = "#28A947";
// Soft selection wash — full accent is too loud for lists / file dialogs.
inline constexpr const char *kHighlight = "#1B3A24";
inline constexpr const char *kHighlightText = "#C8F5D4";
inline constexpr const char *kHighlightHover = "#243D2C";

void applyApplicationFont();
QString stylesheet();

} // namespace Gui::Theme
