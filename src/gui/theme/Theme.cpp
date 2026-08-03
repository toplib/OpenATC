#include "Theme.h"

#include <QApplication>
#include <QFont>
#include <QFontDatabase>

namespace Gui::Theme {

void applyApplicationFont() {
  QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/JetBrainsMono-Regular.ttf"));
  QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/JetBrainsMono-Bold.ttf"));

  QFont font(QStringLiteral("JetBrains Mono"));
  font.setStyleHint(QFont::Monospace);
  font.setPointSize(10);
  QApplication::setFont(font);
}

QString stylesheet() {
  // Named tokens avoid QString::arg() collisions (%1 vs %10…).
  QString css = QStringLiteral(R"(
* {
  font-family: "JetBrains Mono";
}

QMainWindow {
  background-color: @bg;
  color: @text;
}

QWidget#shellCentral {
  background-color: @bg;
  color: @text;
}

QLabel {
  background: transparent;
  border: none;
  color: @text;
}

/* ── Header (top bar) ───────────────────────────────────── */
QFrame#appHeader {
  background-color: @bgSidebar;
  border: none;
  border-bottom: 1px solid @border;
  min-height: 28px;
  max-height: 28px;
}

QPushButton#brandLabel,
QPushButton[class="navItem"] {
  background: transparent;
  border: none;
  border-radius: 4px;
  color: @textMuted;
  font-size: 11px;
  font-weight: 400;
  padding: 3px 10px;
  min-height: 22px;
}

QPushButton#brandLabel:hover,
QPushButton[class="navItem"]:hover {
  color: @text;
  background-color: @bgButton;
}

QPushButton[class="navItem"][active="true"] {
  color: @text;
  background-color: @bgButtonHover;
  font-weight: 600;
}

QPushButton[class="navItem"][active="true"]:hover {
  color: @text;
  background-color: @bgButtonHover;
}

QLineEdit#headerSearch {
  background: transparent;
  border: none;
  color: @text;
  font-size: 10px;
  padding: 2px 4px;
  min-height: 20px;
  max-height: 20px;
  min-width: 112px;
}

QLineEdit#headerSearch:focus {
  color: @text;
}

QPushButton#avatarButton {
  background-color: @bgButton;
  border: 1px solid @border;
  border-radius: 10px;
  padding: 0;
  min-width: 20px;
  max-width: 20px;
  min-height: 20px;
  max-height: 20px;
}

QPushButton#avatarButton:hover {
  background-color: @bgButtonHover;
}

/* ── Splitter (resizable sidebar) ───────────────────────── */
QSplitter::handle:horizontal {
  background-color: @border;
  width: 1px;
  margin: 0;
  padding: 0;
}

QSplitter::handle:horizontal:hover {
  background-color: @textMuted;
  width: 2px;
}

/* ── Sidebar ────────────────────────────────────────────── */
QFrame#projectSidebar {
  background-color: @bgSidebar;
  border: none;
}

QLabel#projectsHeading {
  color: @textMuted;
  font-size: 11px;
  font-weight: 400;
}

QPushButton[class="sidebarAction"] {
  background-color: @bgButton;
  border: 1px solid @border;
  border-radius: 4px;
  color: @text;
  font-size: 11px;
  min-height: 26px;
  padding: 0 10px;
}

QPushButton[class="sidebarAction"]:hover {
  background-color: @bgButtonHover;
  border-color: @textMuted;
}

QFrame[class="projectCard"] {
  background-color: @bgCard;
  border: 1px solid @border;
  border-radius: 5px;
}

QFrame[class="projectCard"]:hover {
  border-color: @textMuted;
}

QLabel[class="projectTitle"] {
  color: @text;
  font-size: 11px;
  font-weight: 600;
}

QLabel[class="projectPath"] {
  color: @textMuted;
  font-size: 9px;
}

QLabel[class="projectMeta"] {
  color: @textDim;
  font-size: 9px;
}

QPushButton[class="projectEdit"] {
  background: transparent;
  border: none;
  border-radius: 4px;
  color: @textMuted;
  font-size: 10px;
  padding: 1px 4px;
}

QPushButton[class="projectEdit"]:hover {
  background-color: @bgButtonHover;
  color: @text;
}

QFrame#tipBox {
  background-color: @bgCard;
  border: 1px solid @border;
  border-radius: 5px;
}

QLabel#tipTitle {
  color: @text;
  font-size: 10px;
  font-weight: 600;
}

QLabel#tipBody {
  color: @textMuted;
  font-size: 9px;
}

QPushButton#tipClose {
  background: transparent;
  border: none;
  border-radius: 4px;
  padding: 2px;
  min-width: 16px;
  max-width: 16px;
  min-height: 16px;
  max-height: 16px;
}

QPushButton#tipClose:hover {
  background-color: @bgButtonHover;
}

/* ── Pages ──────────────────────────────────────────────── */
QWidget[class="pageRoot"], QStackedWidget {
  background-color: @bg;
}

QLabel[class="pageHeroTitle"] {
  color: @text;
  font-size: 36px;
  font-weight: 700;
  letter-spacing: 0;
}

QLabel[class="pageHeroSubtitle"] {
  color: @textMuted;
  font-size: 12px;
}

QPushButton[class="heroAction"] {
  background-color: @bgButton;
  border: 1px solid @border;
  border-radius: 4px;
  color: @text;
  font-size: 12px;
  font-weight: 500;
  min-height: 42px;
  padding: 8px 24px;
}

QPushButton[class="heroAction"]:hover {
  background-color: @bgButtonHover;
  border-color: @textMuted;
}

QPushButton[class="heroAction"]:pressed {
  background-color: @border;
}

QLabel[class="pageTitle"] {
  color: @text;
  font-size: 20px;
  font-weight: 700;
}

QLabel[class="pageBody"] {
  color: @textMuted;
  font-size: 12px;
}

QPushButton[class="primaryAction"] {
  background-color: @bgButton;
  border: 1px solid @border;
  border-radius: 4px;
  color: @text;
  font-size: 11px;
  min-height: 30px;
  padding: 6px 12px;
}

QPushButton[class="primaryAction"]:hover {
  background-color: @bgButtonHover;
}

/* ── Footer ─────────────────────────────────────────────── */
QFrame#statusFooter {
  background-color: @bgSidebar;
  border: none;
  border-top: 1px solid @border;
  min-height: 24px;
  max-height: 24px;
}

QLabel[class="footerText"] {
  color: @textMuted;
  font-size: 9px;
}

QLabel[class="footerOk"] {
  color: @accent;
  font-size: 9px;
  font-weight: 600;
}

QScrollBar:vertical {
  background: transparent;
  width: 8px;
}

QScrollBar::handle:vertical {
  background: @border;
  border-radius: 4px;
  min-height: 24px;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
  height: 0;
}

/* ── Lists / file dialog selection ──────────────────────── */
QAbstractItemView {
  background-color: @bg;
  alternate-background-color: @bgCard;
  color: @text;
  outline: 0;
  selection-background-color: @highlight;
  selection-color: @highlightText;
  border: none;
}

QAbstractItemView::item {
  padding: 4px 8px;
  border-radius: 4px;
  min-height: 22px;
}

QAbstractItemView::item:hover {
  background-color: @highlightHover;
}

QAbstractItemView::item:selected {
  background-color: @highlight;
  color: @highlightText;
}

QAbstractItemView::item:selected:hover {
  background-color: @highlightHover;
  color: @highlightText;
}

QHeaderView::section {
  background-color: @bgSidebar;
  color: @textMuted;
  border: none;
  border-bottom: 1px solid @border;
  padding: 4px 8px;
  font-size: 10px;
}

QFileDialog, QFileDialog QWidget {
  background-color: @bg;
  color: @text;
}

QFileDialog QLabel {
  color: @textMuted;
  font-size: 11px;
}

QFileDialog QLineEdit,
QFileDialog QComboBox {
  background-color: @bgButton;
  border: 1px solid @border;
  border-radius: 4px;
  color: @text;
  padding: 4px 8px;
  min-height: 26px;
  selection-background-color: @highlight;
  selection-color: @highlightText;
}

QFileDialog QLineEdit:focus,
QFileDialog QComboBox:focus {
  border-color: @accent;
}

QFileDialog QComboBox::drop-down {
  border: none;
  width: 22px;
}

QFileDialog QComboBox QAbstractItemView {
  background-color: @bgCard;
  border: 1px solid @border;
  selection-background-color: @highlight;
  selection-color: @highlightText;
}

QFileDialog QPushButton {
  background-color: @bgButton;
  border: 1px solid @border;
  border-radius: 4px;
  color: @text;
  font-size: 11px;
  min-height: 28px;
  padding: 4px 14px;
}

QFileDialog QPushButton:hover {
  background-color: @bgButtonHover;
  border-color: @textMuted;
}

QFileDialog QPushButton:default {
  background-color: @highlight;
  border-color: @accentDim;
  color: @highlightText;
  font-weight: 600;
}

QFileDialog QPushButton:default:hover {
  background-color: @highlightHover;
  border-color: @accent;
}
)");

  const auto put = [&css](const char *token, const char *value) {
    css.replace(QLatin1String(token), QLatin1String(value));
  };
  put("@bgSidebar", kBgSidebar);
  put("@bgButtonHover", kBgButtonHover);
  put("@bgButton", kBgButton);
  put("@bgCard", kBgCard);
  put("@bgRaised", kBgRaised);
  put("@bg", kBg);
  put("@border", kBorder);
  put("@textMuted", kTextMuted);
  put("@textDim", kTextDim);
  put("@text", kText);
  put("@accentDim", kAccentDim);
  put("@accent", kAccent);
  put("@highlightHover", kHighlightHover);
  put("@highlightText", kHighlightText);
  put("@highlight", kHighlight);
  return css;
}

} // namespace Gui::Theme
