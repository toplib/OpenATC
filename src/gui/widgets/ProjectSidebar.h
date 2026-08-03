#pragma once

#include "ProjectCard.h"

#include <QFrame>
#include <QVector>

class QVBoxLayout;

namespace Gui {

class ProjectSidebar : public QFrame {
  Q_OBJECT

public:
  explicit ProjectSidebar(QWidget *parent = nullptr);

  void setProjects(const QVector<ProjectInfo> &projects);
  void addProject(const ProjectInfo &info);

signals:
  void newProjectRequested();
  void openProjectRequested();
  void projectOpened(const ProjectInfo &info);
  void projectEditRequested(const ProjectInfo &info);

private:
  void rebuildList();

  QVBoxLayout *m_listLayout = nullptr;
  QVector<ProjectInfo> m_projects;
};

} // namespace Gui
