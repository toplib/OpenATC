#include "AppHeader.h"

#include "gui/icons/Icons.h"
#include "gui/theme/Theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStyle>

namespace Gui {

AppHeader::AppHeader(QWidget *parent) : QFrame(parent) {
  setObjectName(QStringLiteral("appHeader"));
  setFixedHeight(28);
  setAttribute(Qt::WA_StyledBackground, true);

  auto *root = new QHBoxLayout(this);
  root->setContentsMargins(8, 0, 10, 0);
  root->setSpacing(1);

  // Brand sits in the same nav row (muted); click returns to Activity.
  auto *brand = new QPushButton(QStringLiteral("OpenATC"), this);
  brand->setObjectName(QStringLiteral("brandLabel"));
  brand->setProperty("class", QStringLiteral("navItem"));
  brand->setProperty("active", false);
  brand->setFlat(true);
  brand->setCursor(Qt::PointingHandCursor);
  brand->setAttribute(Qt::WA_StyledBackground, true);
  connect(brand, &QPushButton::clicked, this,
          [this]() { emit pageRequested(PageId::Activity); });
  root->addWidget(brand);

  addNav(QStringLiteral("Activity"), PageId::Activity, root);
  addNav(QStringLiteral("ATC Mode"), PageId::AtcMode, root);
  addNav(QStringLiteral("Pilot Mode"), PageId::PilotMode, root);
  addNav(QStringLiteral("Model Hub"), PageId::ModelHub, root);
  addNav(QStringLiteral("Settings"), PageId::Settings, root);
  addNav(QStringLiteral("Documentation"), PageId::Documentation, root);
  addNav(QStringLiteral("Get Started"), PageId::GetStarted, root);

  root->addStretch(1);

  // Search: icon + placeholder text (matches mock — no heavy pill box)
  auto *searchRow = new QWidget(this);
  auto *searchLayout = new QHBoxLayout(searchRow);
  searchLayout->setContentsMargins(0, 0, 0, 0);
  searchLayout->setSpacing(4);

  auto *searchIcon = new QLabel(searchRow);
  searchIcon->setPixmap(
      Icons::pixmap(Icons::Name::Search, QColor(Theme::kTextMuted), 11));
  searchIcon->setFixedSize(11, 11);
  searchLayout->addWidget(searchIcon);

  m_search = new QLineEdit(searchRow);
  m_search->setObjectName(QStringLiteral("headerSearch"));
  m_search->setPlaceholderText(QStringLiteral("Search..."));
  m_search->setClearButtonEnabled(true);
  connect(m_search, &QLineEdit::returnPressed, this, [this]() {
    const QString q = m_search->text().trimmed();
    if (!q.isEmpty())
      emit searchSubmitted(q);
  });
  searchLayout->addWidget(m_search);
  root->addWidget(searchRow);

  root->addSpacing(6);

  auto *avatar = new QPushButton(this);
  avatar->setObjectName(QStringLiteral("avatarButton"));
  avatar->setIcon(Icons::icon(Icons::Name::User, QColor(Theme::kTextMuted), 11));
  avatar->setIconSize(QSize(11, 11));
  avatar->setCursor(Qt::PointingHandCursor);
  avatar->setToolTip(QStringLiteral("About OpenATC"));
  connect(avatar, &QPushButton::clicked, this, &AppHeader::avatarClicked);
  root->addWidget(avatar);

  setActivePage(PageId::Activity);
}

void AppHeader::addNav(const QString &label, PageId id, QHBoxLayout *layout) {
  auto *btn = new QPushButton(label, this);
  btn->setProperty("class", QStringLiteral("navItem"));
  btn->setProperty("active", false);
  btn->setCursor(Qt::PointingHandCursor);
  btn->setFlat(true);
  btn->setAttribute(Qt::WA_StyledBackground, true);
  layout->addWidget(btn);
  m_navButtons.insert(id, btn);
  connect(btn, &QPushButton::clicked, this, [this, id]() { emit pageRequested(id); });
}

void AppHeader::setActivePage(PageId id) {
  for (auto it = m_navButtons.begin(); it != m_navButtons.end(); ++it) {
    const bool active = (it.key() == id);
    it.value()->setProperty("active", active);
    it.value()->style()->unpolish(it.value());
    it.value()->style()->polish(it.value());
    it.value()->update();
  }
}

QString AppHeader::searchText() const {
  return m_search ? m_search->text() : QString();
}

void AppHeader::clearSearch() {
  if (m_search)
    m_search->clear();
}

} // namespace Gui
