#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class OpenATCMainWindow;
}
QT_END_NAMESPACE

class HomeWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit HomeWindow(QWidget *parent = nullptr);
  ~HomeWindow() override;

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
  // Navigation
  void onAtcMode();
  void onPilotMode();
  void onModelHub();
  void onSettingsNav();

  // Header toolbar
  void onHeaderSettings();
  void onHeaderTerminal();
  void onHeaderHelp();

  // Search
  void onSearchCommand();

  // Sidebar projects
  void onNewProject();
  void onOpenProject();

  // Recent sessions
  void onSession1();
  void onSession2();
  void onSession3();

  // Action cards
  void onNewSimulation();
  void onTrainingScenarios();
  void onHistoricalScenarios();
  void onReplayBrowser();
  void onPluginMarketplace();
  void onModelManagement();
  void onSystemSettings();

  // Footer
  void onResources();
  void onNetworkStatus();

private:
  void connectSignals();
  void installCardFilters();

  Ui::OpenATCMainWindow *ui;
};
