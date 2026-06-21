#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QScreen>
#include <spdlog/spdlog.h>

#include "gui/home/HomeWindow.h"

static void applyDarkPalette(QApplication &app) {
  QPalette p;
  p.setColor(QPalette::Window, QColor("#131313"));
  p.setColor(QPalette::WindowText, QColor("#e4e2e1"));
  p.setColor(QPalette::Base, QColor("#1b1c1c"));
  p.setColor(QPalette::AlternateBase, QColor("#1f2020"));
  p.setColor(QPalette::ToolTipBase, QColor("#2a2a2a"));
  p.setColor(QPalette::ToolTipText, QColor("#e4e2e1"));
  p.setColor(QPalette::Text, QColor("#e4e2e1"));
  p.setColor(QPalette::Button, QColor("#1f2020"));
  p.setColor(QPalette::ButtonText, QColor("#e4e2e1"));
  p.setColor(QPalette::BrightText, QColor("#00e639"));
  p.setColor(QPalette::Link, QColor("#00e639"));
  p.setColor(QPalette::Highlight, QColor("#00e639"));
  p.setColor(QPalette::HighlightedText, QColor("#002203"));
  p.setColor(QPalette::PlaceholderText, QColor("#84967e"));
  app.setPalette(p);
}

int main(int argc, char *argv[]) {
  spdlog::info("asd");
  QApplication app(argc, argv);
  app.setApplicationName("OpenATC");
  app.setOrganizationName("OpenATC");
  app.setApplicationVersion("0.1.0");

  applyDarkPalette(app);

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
