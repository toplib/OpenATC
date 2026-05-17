#pragma once
#include <QApplication>
#include <QObject>

#include "main/MainWindow.h"
#include "settings/Settings.h"

namespace GUI {
    class Manager : public QObject{
        Q_OBJECT

    public:
        Manager(int argc, char *argv[]);
        ~Manager();

        int run();
    public slots:
        void showSettings();

    private:
        QApplication m_application;
        std::unique_ptr<MainWindow> m_mainWindow;
        std::unique_ptr<Settings> m_settings;
    };
}
