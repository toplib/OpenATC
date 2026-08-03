#include "ProjectCard.h"

#include "gui/icons/Icons.h"
#include "gui/theme/Theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

namespace Gui {

static QWidget *metaChip(Icons::Name icon, const QString &text, QWidget *parent) {
  auto *wrap = new QWidget(parent);
  auto *row = new QHBoxLayout(wrap);
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(4);

  auto *iconLabel = new QLabel(wrap);
  iconLabel->setPixmap(Icons::pixmap(icon, QColor(Theme::kTextDim), 12));
  iconLabel->setFixedSize(12, 12);
  row->addWidget(iconLabel);

  auto *textLabel = new QLabel(text, wrap);
  textLabel->setProperty("class", QStringLiteral("projectMeta"));
  row->addWidget(textLabel);
  return wrap;
}

ProjectCard::ProjectCard(const ProjectInfo &info, QWidget *parent)
    : QFrame(parent), m_info(info) {
  setProperty("class", QStringLiteral("projectCard"));
  setAttribute(Qt::WA_StyledBackground, true);
  setCursor(Qt::PointingHandCursor);
  setMinimumHeight(58);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(8, 7, 8, 7);
  root->setSpacing(3);

  auto *top = new QHBoxLayout();
  top->setSpacing(4);

  auto *title = new QLabel(info.title, this);
  title->setProperty("class", QStringLiteral("projectTitle"));
  title->setWordWrap(true);
  top->addWidget(title, 1);

  auto *edit = new QPushButton(QStringLiteral(" Edit"), this);
  edit->setProperty("class", QStringLiteral("projectEdit"));
  edit->setIcon(Icons::icon(Icons::Name::Pencil, QColor(Theme::kTextMuted), 11));
  edit->setIconSize(QSize(11, 11));
  edit->setCursor(Qt::PointingHandCursor);
  edit->setToolTip(QStringLiteral("Edit project"));
  connect(edit, &QPushButton::clicked, this, [this]() { emit editRequested(m_info); });
  top->addWidget(edit, 0, Qt::AlignTop);

  root->addLayout(top);

  auto *path = new QLabel(info.fileName, this);
  path->setProperty("class", QStringLiteral("projectPath"));
  root->addWidget(path);

  auto *meta = new QHBoxLayout();
  meta->setSpacing(10);
  meta->addWidget(metaChip(Icons::Name::Send, QString::number(info.aircraftCount), this));
  meta->addWidget(metaChip(Icons::Name::Clock, info.duration, this));
  meta->addStretch(1);

  auto *when = new QLabel(info.lastOpened, this);
  when->setProperty("class", QStringLiteral("projectMeta"));
  meta->addWidget(when);
  root->addLayout(meta);
}

void ProjectCard::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    emit opened(m_info);
  QFrame::mouseReleaseEvent(event);
}

} // namespace Gui
