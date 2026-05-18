#include "gui/Manager.h"

int main(int argc, char *argv[]) {
    GUI::Manager manager(argc, argv);
    return manager.run();
}