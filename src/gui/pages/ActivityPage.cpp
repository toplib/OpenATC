#include "ActivityPage.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Gui {

static QPushButton *makeHeroButton(const QString &text, QWidget *parent) {
  auto *btn = new QPushButton(text, parent);
  btn->setProperty("class", QStringLiteral("heroAction"));
  btn->setCursor(Qt::PointingHandCursor);
  btn->setMinimumWidth(420);
  btn->setMaximumWidth(460);
  return btn;
}

ActivityPage::ActivityPage(QWidget *parent) : IPage(parent) {
  setProperty("class", QStringLiteral("pageRoot"));
  setAttribute(Qt::WA_StyledBackground, true);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(40, 40, 40, 40);
  root->setSpacing(10);
  root->addStretch(2);

  auto *title = new QLabel(QStringLiteral("OpenATC"), this);
  title->setProperty("class", QStringLiteral("pageHeroTitle"));
  title->setAlignment(Qt::AlignHCenter);
  root->addWidget(title);

  auto *subtitle =
      new QLabel(QStringLiteral("Choose where you want to get started with"), this);
  subtitle->setProperty("class", QStringLiteral("pageHeroSubtitle"));
  subtitle->setAlignment(Qt::AlignHCenter);
  root->addWidget(subtitle);
  root->addSpacing(40);

  auto *center = new QVBoxLayout();
  center->setSpacing(14);
  center->setAlignment(Qt::AlignHCenter);

  auto *simulate = makeHeroButton(QStringLiteral("Simulate any scenario"), this);
  connect(simulate, &QPushButton::clicked, this, &ActivityPage::simulateRequested);
  center->addWidget(simulate, 0, Qt::AlignHCenter);

  auto *market =
      makeHeroButton(QStringLiteral("Explore plugin marketplace"), this);
  connect(market, &QPushButton::clicked, this, &ActivityPage::marketplaceRequested);
  center->addWidget(market, 0, Qt::AlignHCenter);

  auto *tutorial =
      makeHeroButton(QStringLiteral("Get started with tutorial"), this);
  connect(tutorial, &QPushButton::clicked, this, &ActivityPage::tutorialRequested);
  center->addWidget(tutorial, 0, Qt::AlignHCenter);

  auto *about = makeHeroButton(QStringLiteral("About this program"), this);
  connect(about, &QPushButton::clicked, this, &ActivityPage::aboutRequested);
  center->addWidget(about, 0, Qt::AlignHCenter);

  root->addLayout(center);
  root->addStretch(3);
}

} // namespace Gui
