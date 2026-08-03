#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QScreen>
#include <spdlog/spdlog.h>

#include "gui/home/HomeWindow.h"
#include "gui/theme/Theme.h"

static void applyDarkPalette(QApplication &app) {
  QPalette p;
  p.setColor(QPalette::Window, QColor(Gui::Theme::kBg));
  p.setColor(QPalette::WindowText, QColor(Gui::Theme::kText));
  p.setColor(QPalette::Base, QColor(Gui::Theme::kBgSidebar));
  p.setColor(QPalette::AlternateBase, QColor(Gui::Theme::kBgRaised));
  p.setColor(QPalette::ToolTipBase, QColor(Gui::Theme::kBgCard));
  p.setColor(QPalette::ToolTipText, QColor(Gui::Theme::kText));
  p.setColor(QPalette::Text, QColor(Gui::Theme::kText));
  p.setColor(QPalette::Button, QColor(Gui::Theme::kBgButton));
  p.setColor(QPalette::ButtonText, QColor(Gui::Theme::kText));
  p.setColor(QPalette::BrightText, QColor(Gui::Theme::kAccent));
  p.setColor(QPalette::Link, QColor(Gui::Theme::kAccent));
  p.setColor(QPalette::Highlight, QColor(Gui::Theme::kHighlight));
  p.setColor(QPalette::HighlightedText, QColor(Gui::Theme::kHighlightText));
  p.setColor(QPalette::PlaceholderText, QColor(Gui::Theme::kTextMuted));
  app.setPalette(p);
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("OpenATC");
  app.setOrganizationName("OpenATC");
  app.setApplicationVersion("1.0 LTS");

  Gui::Theme::applyApplicationFont();
  applyDarkPalette(app);
  app.setStyleSheet(Gui::Theme::stylesheet());

  HomeWindow window;

  if (const QScreen *screen = QApplication::primaryScreen()) {
    const QRect avail = screen->availableGeometry();
    window.resize(qMin(window.width(), avail.width()),
                  qMin(window.height(), avail.height()));
    window.move((avail.width() - window.width()) / 2,
                (avail.height() - window.height()) / 2);
  }

  window.show();
  return app.exec();
}