#include "ProjectSidebar.h"
#include "TipBox.h"

#include "gui/icons/Icons.h"
#include "gui/theme/Theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace Gui {

ProjectSidebar::ProjectSidebar(QWidget *parent) : QFrame(parent) {
  setObjectName(QStringLiteral("projectSidebar"));
  setAttribute(Qt::WA_StyledBackground, true);
  setMinimumWidth(200);
  setMaximumWidth(480);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(10, 10, 10, 10);
  root->setSpacing(8);

  auto *heading = new QLabel(QStringLiteral("Projects"), this);
  heading->setObjectName(QStringLiteral("projectsHeading"));
  root->addWidget(heading);

  auto *actions = new QHBoxLayout();
  actions->setSpacing(6);

  auto *newBtn = new QPushButton(QStringLiteral(" New"), this);
  newBtn->setProperty("class", QStringLiteral("sidebarAction"));
  newBtn->setIcon(Icons::icon(Icons::Name::Plus, QColor(Theme::kText), 11));
  newBtn->setIconSize(QSize(11, 11));
  newBtn->setCursor(Qt::PointingHandCursor);
  connect(newBtn, &QPushButton::clicked, this, &ProjectSidebar::newProjectRequested);
  actions->addWidget(newBtn);

  auto *openBtn = new QPushButton(QStringLiteral(" Open"), this);
  openBtn->setProperty("class", QStringLiteral("sidebarAction"));
  openBtn->setIcon(Icons::icon(Icons::Name::FolderOpen, QColor(Theme::kText), 11));
  openBtn->setIconSize(QSize(11, 11));
  openBtn->setCursor(Qt::PointingHandCursor);
  connect(openBtn, &QPushButton::clicked, this, &ProjectSidebar::openProjectRequested);
  actions->addWidget(openBtn);
  root->addLayout(actions);

  auto *scroll = new QScrollArea(this);
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll->setStyleSheet(QStringLiteral(
      "QScrollArea { background: transparent; border: none; }"));

  auto *listHost = new QWidget(scroll);
  listHost->setStyleSheet(QStringLiteral("background: transparent;"));
  m_listLayout = new QVBoxLayout(listHost);
  m_listLayout->setContentsMargins(0, 0, 0, 0);
  m_listLayout->setSpacing(6);
  m_listLayout->addStretch(1);
  scroll->setWidget(listHost);
  root->addWidget(scroll, 1);

  auto *tip = new TipBox(this);
  root->addWidget(tip);

  setProjects({
      ProjectInfo{
          QStringLiteral("umms-clean"),
          QStringLiteral("UMMS - Clean weather"),
          QStringLiteral("umms_clean_weather.osf"),
          12,
          QStringLiteral("02:31:44"),
          QStringLiteral("Yesterday"),
      },
  });
}

void ProjectSidebar::setProjects(const QVector<ProjectInfo> &projects) {
  m_projects = projects;
  rebuildList();
}

void ProjectSidebar::addProject(const ProjectInfo &info) {
  m_projects.prepend(info);
  rebuildList();
}

void ProjectSidebar::rebuildList() {
  while (m_listLayout->count() > 0) {
    QLayoutItem *item = m_listLayout->takeAt(0);
    if (item->widget())
      item->widget()->deleteLater();
    delete item;
  }

  for (const ProjectInfo &info : m_projects) {
    auto *card = new ProjectCard(info, this);
    connect(card, &ProjectCard::opened, this, &ProjectSidebar::projectOpened);
    connect(card, &ProjectCard::editRequested, this,
            &ProjectSidebar::projectEditRequested);
    m_listLayout->addWidget(card);
  }
  m_listLayout->addStretch(1);
}

} // namespace Gui
