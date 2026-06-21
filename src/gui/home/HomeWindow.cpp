#include "HomeWindow.h"
#include "ui_openatc.h"

#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>

HomeWindow::HomeWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::OpenATCMainWindow) {
  ui->setupUi(this);
  setWindowTitle("OpenATC");

  connectSignals();
  installCardFilters();
}

HomeWindow::~HomeWindow() { delete ui; }

// ============================================================================
//  Signal wiring
// ============================================================================

void HomeWindow::connectSignals() {
  // ── Navigation tabs ─────────────────────────────────────────────────
  connect(ui->navButtonActive, &QPushButton::clicked, this,
          &HomeWindow::onAtcMode);
  connect(ui->navPilotMode, &QPushButton::clicked, this,
          &HomeWindow::onPilotMode);
  connect(ui->navModelHub, &QPushButton::clicked, this,
          &HomeWindow::onModelHub);
  connect(ui->navSettings, &QPushButton::clicked, this,
          &HomeWindow::onSettingsNav);

  // ── Header toolbar ──────────────────────────────────────────────────
  connect(ui->headerSettingsButton, &QPushButton::clicked, this,
          &HomeWindow::onHeaderSettings);
  connect(ui->headerTerminalButton, &QPushButton::clicked, this,
          &HomeWindow::onHeaderTerminal);
  connect(ui->headerHelpButton, &QPushButton::clicked, this,
          &HomeWindow::onHeaderHelp);

  // ── Search bar ──────────────────────────────────────────────────────
  connect(ui->searchBar, &QLineEdit::returnPressed, this,
          &HomeWindow::onSearchCommand);

  // ── Sidebar buttons ─────────────────────────────────────────────────
  connect(ui->newProjectButton, &QPushButton::clicked, this,
          &HomeWindow::onNewProject);
  connect(ui->openProjectButton, &QPushButton::clicked, this,
          &HomeWindow::onOpenProject);

  // ── Footer links ────────────────────────────────────────────────────
  connect(ui->footerResources, &QPushButton::clicked, this,
          &HomeWindow::onResources);
  connect(ui->footerNetworkStatus, &QPushButton::clicked, this,
          &HomeWindow::onNetworkStatus);
}

void HomeWindow::installCardFilters() {
  // Action cards (QFrame – need eventFilter for click handling)
  ui->cardNewSimulation->installEventFilter(this);
  ui->cardTraining->installEventFilter(this);
  ui->cardHistorical->installEventFilter(this);
  ui->cardReplay->installEventFilter(this);
  ui->cardPlugins->installEventFilter(this);
  ui->cardModels->installEventFilter(this);
  ui->cardSystemSettings->installEventFilter(this);

  // Recent session items
  ui->recentSession1->installEventFilter(this);
  ui->recentSession2->installEventFilter(this);
  ui->recentSession3->installEventFilter(this);
}

// ============================================================================
//  Event filter — handles clicks on QFrame-based cards
// ============================================================================

bool HomeWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::MouseButtonRelease) {
    auto *me = static_cast<QMouseEvent *>(event);
    if (me->button() != Qt::LeftButton) {
      return QMainWindow::eventFilter(obj, event);
    }

    // Action cards
    if (obj == ui->cardNewSimulation) {
      onNewSimulation();
      return true;
    }
    if (obj == ui->cardTraining) {
      onTrainingScenarios();
      return true;
    }
    if (obj == ui->cardHistorical) {
      onHistoricalScenarios();
      return true;
    }
    if (obj == ui->cardReplay) {
      onReplayBrowser();
      return true;
    }
    if (obj == ui->cardPlugins) {
      onPluginMarketplace();
      return true;
    }
    if (obj == ui->cardModels) {
      onModelManagement();
      return true;
    }
    if (obj == ui->cardSystemSettings) {
      onSystemSettings();
      return true;
    }

    // Recent sessions
    if (obj == ui->recentSession1) {
      onSession1();
      return true;
    }
    if (obj == ui->recentSession2) {
      onSession2();
      return true;
    }
    if (obj == ui->recentSession3) {
      onSession3();
      return true;
    }
  }

  return QMainWindow::eventFilter(obj, event);
}

// ============================================================================
//  Slots — navigation
// ============================================================================

void HomeWindow::onAtcMode() { qDebug() << "[Nav] ATC Mode selected"; }
void HomeWindow::onPilotMode() { qDebug() << "[Nav] Pilot Mode selected"; }
void HomeWindow::onModelHub() { qDebug() << "[Nav] Model Hub selected"; }
void HomeWindow::onSettingsNav() { qDebug() << "[Nav] Settings selected"; }

// ============================================================================
//  Slots — header toolbar
// ============================================================================

void HomeWindow::onHeaderSettings() { qDebug() << "[Header] Settings clicked"; }
void HomeWindow::onHeaderTerminal() { qDebug() << "[Header] Terminal toggled"; }
void HomeWindow::onHeaderHelp() { qDebug() << "[Header] Help opened"; }

// ============================================================================
//  Slots — search
// ============================================================================

void HomeWindow::onSearchCommand() {
  const QString cmd = ui->searchBar->text().trimmed();
  if (cmd.isEmpty()) {
    return;
  }

  qDebug() << "[CMD]" << cmd;
  ui->searchBar->clear();
}

// ============================================================================
//  Slots — sidebar
// ============================================================================

void HomeWindow::onNewProject() { qDebug() << "[Sidebar] New project"; }
void HomeWindow::onOpenProject() { qDebug() << "[Sidebar] Open project"; }

void HomeWindow::onSession1() {
  qDebug() << "[Session] KJFK_HEAVY_RUSH_04A opened";
}
void HomeWindow::onSession2() {
  qDebug() << "[Session] KSFO_FOG_RECOVERY opened";
}
void HomeWindow::onSession3() { qDebug() << "[Session] EGLL_NIGHT_OPS opened"; }

// ============================================================================
//  Slots — action cards
// ============================================================================

void HomeWindow::onNewSimulation() {
  qDebug() << "[Action] New Simulation launched";
  // TODO: open simulation setup dialog
}

void HomeWindow::onTrainingScenarios() {
  qDebug() << "[Action] Training Scenarios opened";
}

void HomeWindow::onHistoricalScenarios() {
  qDebug() << "[Action] Historical Scenarios opened";
}

void HomeWindow::onReplayBrowser() {
  qDebug() << "[Action] Replay Browser opened";
}

void HomeWindow::onPluginMarketplace() {
  qDebug() << "[Action] Plugin Marketplace opened";
}

void HomeWindow::onModelManagement() {
  qDebug() << "[Action] Model Management opened";
}

void HomeWindow::onSystemSettings() {
  qDebug() << "[Action] System Settings opened";
}

// ============================================================================
//  Slots — footer
// ============================================================================

void HomeWindow::onResources() { qDebug() << "[Footer] Resources opened"; }
void HomeWindow::onNetworkStatus() {
  qDebug() << "[Footer] Network status opened";
}
