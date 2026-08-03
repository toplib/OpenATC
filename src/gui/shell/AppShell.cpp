#include "AppShell.h"

#include "gui/pages/ActivityPage.h"
#include "gui/pages/ContentPages.h"
#include "gui/pages/SettingsPage.h"
#include "gui/widgets/AppHeader.h"
#include "gui/widgets/ProjectSidebar.h"
#include "gui/widgets/StatusFooter.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace Gui {

AppShell::AppShell(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(QStringLiteral("OpenATC"));
  resize(1280, 800);

  auto *central = new QWidget(this);
  central->setObjectName(QStringLiteral("shellCentral"));
  setCentralWidget(central);

  auto *root = new QVBoxLayout(central);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // Top navigation bar — always visible
  m_header = new AppHeader(central);
  root->addWidget(m_header);

  // Body: resizable sidebar + page stack
  m_splitter = new QSplitter(Qt::Horizontal, central);
  m_splitter->setObjectName(QStringLiteral("mainSplitter"));
  m_splitter->setChildrenCollapsible(false);
  m_splitter->setHandleWidth(1);

  m_sidebar = new ProjectSidebar(m_splitter);
  m_stack = new QStackedWidget(m_splitter);

  m_splitter->addWidget(m_sidebar);
  m_splitter->addWidget(m_stack);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  m_splitter->setSizes({200, 1080});
  root->addWidget(m_splitter, 1);

  // Bottom status bar
  m_footer = new StatusFooter(central);
  m_footer->setVersion(QStringLiteral("1.0 LTS"));
  root->addWidget(m_footer);

  auto *activity = new ActivityPage(m_stack);
  registerPage(activity);
  registerPage(new AtcModePage(m_stack));
  registerPage(new PilotModePage(m_stack));
  registerPage(new ModelHubPage(m_stack));
  registerPage(new SettingsPage(m_stack));
  registerPage(new DocumentationPage(m_stack));
  registerPage(new GetStartedPage(m_stack));

  connect(m_header, &AppHeader::pageRequested, this, &AppShell::navigateTo);
  connect(m_header, &AppHeader::searchSubmitted, this, &AppShell::onSearch);
  connect(m_header, &AppHeader::avatarClicked, this, &AppShell::onAbout);

  connect(m_sidebar, &ProjectSidebar::newProjectRequested, this,
          &AppShell::onNewProject);
  connect(m_sidebar, &ProjectSidebar::openProjectRequested, this,
          &AppShell::onOpenProject);
  connect(m_sidebar, &ProjectSidebar::projectOpened, this,
          &AppShell::onProjectOpened);
  connect(m_sidebar, &ProjectSidebar::projectEditRequested, this,
          &AppShell::onProjectEdit);

  connect(activity, &ActivityPage::simulateRequested, this, &AppShell::onSimulate);
  connect(activity, &ActivityPage::marketplaceRequested, this,
          &AppShell::onMarketplace);
  connect(activity, &ActivityPage::tutorialRequested, this, &AppShell::onTutorial);
  connect(activity, &ActivityPage::aboutRequested, this, &AppShell::onAbout);

  if (auto *atc = qobject_cast<AtcModePage *>(pageFor(PageId::AtcMode))) {
    connect(atc, &ContentPage::primaryActionTriggered, this, &AppShell::onSimulate);
  }
  if (auto *pilot = qobject_cast<PilotModePage *>(pageFor(PageId::PilotMode))) {
    connect(pilot, &ContentPage::primaryActionTriggered, this, [this]() {
      showInfo(QStringLiteral("Pilot Mode"),
               QStringLiteral("Pilot session launcher will open here."));
    });
  }
  if (auto *models = qobject_cast<ModelHubPage *>(pageFor(PageId::ModelHub))) {
    connect(models, &ContentPage::primaryActionTriggered, this, [this]() {
      showInfo(QStringLiteral("Model Hub"),
               QStringLiteral("Model browser will open here."));
    });
  }
  if (auto *docs = qobject_cast<DocumentationPage *>(pageFor(PageId::Documentation))) {
    connect(docs, &ContentPage::primaryActionTriggered, this, [this]() {
      showInfo(QStringLiteral("Documentation"),
               QStringLiteral("Quickstart guide will open here."));
    });
  }
  if (auto *started = qobject_cast<GetStartedPage *>(pageFor(PageId::GetStarted))) {
    connect(started, &ContentPage::primaryActionTriggered, this,
            &AppShell::onTutorial);
  }

  navigateTo(PageId::Activity);
}

void AppShell::registerPage(IPage *page) { m_stack->addWidget(page); }

IPage *AppShell::pageFor(PageId id) const {
  for (int i = 0; i < m_stack->count(); ++i) {
    if (auto *page = qobject_cast<IPage *>(m_stack->widget(i))) {
      if (page->pageId() == id)
        return page;
    }
  }
  return nullptr;
}

void AppShell::navigateTo(PageId id) {
  IPage *page = pageFor(id);
  if (!page)
    return;

  if (IPage *current = pageFor(m_current))
    current->onDeactivated();

  m_stack->setCurrentWidget(page);
  page->onActivated();
  m_current = id;
  m_header->setActivePage(id);
}

void AppShell::onSearch(const QString &query) {
  const QString q = query.trimmed().toLower();
  m_header->clearSearch();

  if (q.contains(QLatin1String("atc"))) {
    navigateTo(PageId::AtcMode);
    return;
  }
  if (q.contains(QLatin1String("pilot"))) {
    navigateTo(PageId::PilotMode);
    return;
  }
  if (q.contains(QLatin1String("model"))) {
    navigateTo(PageId::ModelHub);
    return;
  }
  if (q.contains(QLatin1String("setting"))) {
    navigateTo(PageId::Settings);
    return;
  }
  if (q.contains(QLatin1String("doc"))) {
    navigateTo(PageId::Documentation);
    return;
  }
  if (q.contains(QLatin1String("start")) || q.contains(QLatin1String("tutorial"))) {
    navigateTo(PageId::GetStarted);
    return;
  }
  if (q.contains(QLatin1String("activ")) || q.contains(QLatin1String("home"))) {
    navigateTo(PageId::Activity);
    return;
  }

  showInfo(QStringLiteral("Search"),
           QStringLiteral("No match for \"%1\". Try: atc, pilot, models, "
                          "settings, docs, tutorial.")
               .arg(query));
}

void AppShell::onNewProject() {
  bool ok = false;
  const QString name = QInputDialog::getText(
      this, QStringLiteral("New project"),
      QStringLiteral("Project name:"), QLineEdit::Normal,
      QStringLiteral("New scenario"), &ok);
  if (!ok || name.trimmed().isEmpty())
    return;

  QString file = name.trimmed().toLower();
  file.replace(QLatin1Char(' '), QLatin1Char('_'));
  if (!file.endsWith(QLatin1String(".cs")))
    file.append(QStringLiteral(".cs"));

  m_sidebar->addProject(ProjectInfo{
      name.trimmed().toLower().replace(QLatin1Char(' '), QLatin1Char('-')),
      name.trimmed(),
      file,
      0,
      QStringLiteral("00:00:00"),
      QStringLiteral("Just now"),
  });
}

void AppShell::onOpenProject() {
  const QString path = QFileDialog::getOpenFileName(
      this, QStringLiteral("Open project"), QString(),
      QStringLiteral("Scenario files (*.cs *.json *.toml);;All files (*)"));
  if (path.isEmpty())
    return;

  const QFileInfo fi(path);
  m_sidebar->addProject(ProjectInfo{
      fi.completeBaseName(),
      fi.completeBaseName(),
      fi.fileName(),
      0,
      QStringLiteral("00:00:00"),
      QStringLiteral("Just now"),
  });
  showInfo(QStringLiteral("Project opened"),
           QStringLiteral("Loaded \"%1\".").arg(fi.fileName()));
}

void AppShell::onProjectOpened(const ProjectInfo &info) {
  navigateTo(PageId::AtcMode);
  showInfo(QStringLiteral("Project"),
           QStringLiteral("Opening \"%1\" (%2).")
               .arg(info.title, info.fileName));
}

void AppShell::onProjectEdit(const ProjectInfo &info) {
  showInfo(QStringLiteral("Edit project"),
           QStringLiteral("Editor for \"%1\" will open here.").arg(info.title));
}

void AppShell::onSimulate() {
  navigateTo(PageId::AtcMode);
  showInfo(QStringLiteral("Simulate"),
           QStringLiteral("Scenario setup will open here."));
}

void AppShell::onMarketplace() {
  showInfo(QStringLiteral("Plugin marketplace"),
           QStringLiteral("Marketplace browser will open here."));
}

void AppShell::onTutorial() { navigateTo(PageId::GetStarted); }

void AppShell::onAbout() {
  QMessageBox::about(
      this, QStringLiteral("About OpenATC"),
      QStringLiteral(
          "<h3>OpenATC 1.0 LTS</h3>"
          "<p>Open-source ATC training simulator with Whisper.cpp, Llama.cpp, "
          "and Sherpa-ONNX backends.</p>"));
}

void AppShell::showInfo(const QString &title, const QString &text) {
  QMessageBox::information(this, title, text);
}

} // namespace Gui
