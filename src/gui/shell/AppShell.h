#pragma once

#include "gui/navigation/PageId.h"
#include "gui/widgets/ProjectCard.h"

#include <QMainWindow>

class QStackedWidget;
class QSplitter;

namespace Gui {

class AppHeader;
class ProjectSidebar;
class StatusFooter;
class IPage;

class AppShell : public QMainWindow {
  Q_OBJECT

public:
  explicit AppShell(QWidget *parent = nullptr);

  void navigateTo(PageId id);

private slots:
  void onSearch(const QString &query);
  void onNewProject();
  void onOpenProject();
  void onProjectOpened(const ProjectInfo &info);
  void onProjectEdit(const ProjectInfo &info);
  void onSimulate();
  void onMarketplace();
  void onTutorial();
  void onAbout();

private:
  void registerPage(IPage *page);
  IPage *pageFor(PageId id) const;
  void showInfo(const QString &title, const QString &text);

  AppHeader *m_header = nullptr;
  ProjectSidebar *m_sidebar = nullptr;
  StatusFooter *m_footer = nullptr;
  QStackedWidget *m_stack = nullptr;
  QSplitter *m_splitter = nullptr;
  PageId m_current = PageId::Activity;
};

} // namespace Gui
