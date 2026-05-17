#include "Manager.h"

#include <spdlog/spdlog.h>
#include <QTimer>

namespace GUI {
    Manager::Manager(int argc, char *argv[]) : m_application(argc, argv), m_mainWindow(std::make_unique<MainWindow>()) {
        spdlog::info("Initializing GUI");
        connect(m_mainWindow.get(), &MainWindow::settingsRequested, this, &Manager::showSettings);
    }

    Manager::~Manager() {
        spdlog::info("Destroying GUI");
    }


    void Manager::showSettings() {
        if (!m_settings) {
            m_settings = std::make_unique<Settings>(m_mainWindow.get());
        }
        m_settings->exec();
    }

    int Manager::run() {
        QTimer::singleShot(0, m_mainWindow.get(), &QWidget::show);
        return m_application.exec();
    }
}
