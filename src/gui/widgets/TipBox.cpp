#include "TipBox.h"

#include "gui/icons/Icons.h"
#include "gui/theme/Theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Gui {

TipBox::TipBox(QWidget *parent) : QFrame(parent) {
  setObjectName(QStringLiteral("tipBox"));
  setAttribute(Qt::WA_StyledBackground, true);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(8, 7, 8, 8);
  root->setSpacing(4);

  auto *header = new QHBoxLayout();
  m_title = new QLabel(QStringLiteral("Todays tip:"), this);
  m_title->setObjectName(QStringLiteral("tipTitle"));
  header->addWidget(m_title, 1);

  auto *close = new QPushButton(this);
  close->setObjectName(QStringLiteral("tipClose"));
  close->setIcon(Icons::icon(Icons::Name::Close, QColor(Theme::kTextMuted), 10));
  close->setIconSize(QSize(10, 10));
  close->setCursor(Qt::PointingHandCursor);
  close->setToolTip(QStringLiteral("Dismiss tip"));
  connect(close, &QPushButton::clicked, this, [this]() {
    hide();
    emit dismissed();
  });
  header->addWidget(close);
  root->addLayout(header);

  m_body = new QLabel(this);
  m_body->setObjectName(QStringLiteral("tipBody"));
  m_body->setWordWrap(true);
  root->addWidget(m_body);

  setTip(QStringLiteral("Todays tip:"),
         QStringLiteral("blah blah blah..."));
}

void TipBox::setTip(const QString &title, const QString &body) {
  m_title->setText(title);
  m_body->setText(body);
}

} // namespace Gui
