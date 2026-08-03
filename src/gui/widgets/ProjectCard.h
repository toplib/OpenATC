#pragma once

#include <QFrame>
#include <QString>

namespace Gui {

struct ProjectInfo {
  QString id;
  QString title;
  QString fileName;
  int aircraftCount = 0;
  QString duration;
  QString lastOpened;
};

class ProjectCard : public QFrame {
  Q_OBJECT

public:
  explicit ProjectCard(const ProjectInfo &info, QWidget *parent = nullptr);

  const ProjectInfo &info() const { return m_info; }

signals:
  void opened(const ProjectInfo &info);
  void editRequested(const ProjectInfo &info);

protected:
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  ProjectInfo m_info;
};

} // namespace Gui
