#include "SettingsPage.h"

#include "gui/settings/Settings.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Gui {

SettingsPage::SettingsPage(QWidget *parent) : IPage(parent) {
  setProperty("class", QStringLiteral("pageRoot"));
  setAttribute(Qt::WA_StyledBackground, true);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(32, 32, 32, 32);
  root->setSpacing(12);

  auto *title = new QLabel(QStringLiteral("Settings"), this);
  title->setProperty("class", QStringLiteral("pageTitle"));
  root->addWidget(title);

  auto *body = new QLabel(
      QStringLiteral(
          "Configure model paths, cache directories, and application preferences."),
      this);
  body->setProperty("class", QStringLiteral("pageBody"));
  body->setWordWrap(true);
  root->addWidget(body);

  auto *openDlg = new QPushButton(QStringLiteral("Open settings dialog"), this);
  openDlg->setProperty("class", QStringLiteral("primaryAction"));
  openDlg->setCursor(Qt::PointingHandCursor);
  connect(openDlg, &QPushButton::clicked, this, [this]() {
    Settings dialog(this);
    dialog.exec();
  });
  root->addWidget(openDlg, 0, Qt::AlignLeft);
  root->addStretch(1);
}

} // namespace Gui
