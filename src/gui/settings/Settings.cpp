#include "Settings.h"

#include "ui_Settings.h"

Settings::Settings(QWidget *parent) : QDialog(parent), m_ui(new Ui::Settings) {
    m_ui->setupUi(this);
}

Settings::~Settings() {
    delete m_ui;
}