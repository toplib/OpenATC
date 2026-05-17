#include <iostream>
#include <QMessageBox>

#include "gui/Manager.h"
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    GUI::Manager manager(argc, argv);
    //manager.show();
    return manager.run();
    spdlog::info("after run");
}