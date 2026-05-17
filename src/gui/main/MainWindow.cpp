#include "MainWindow.h"

#include "ui_MainWindow.h"
#include <QMessageBox>

#include "Constants.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow) {
    m_ui->setupUi(this);
    this->setWindowTitle("SimpleATC");

    QAction* aboutAction = new QAction(tr("&About"), this);
    aboutAction->setShortcut(QKeySequence());
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "SimpleATC about", ABOUT_SECTION.data());
    });

    QAction* settingsAction = new QAction(tr("&Settings"), this);
    settingsAction->setShortcut(QKeySequence());
    connect(settingsAction, &QAction::triggered, this, &MainWindow::settingsRequested);


    m_ui->menuAbout->addAction(aboutAction);
    m_ui->menuTools->addAction(settingsAction);
}

MainWindow::~MainWindow() {
    delete m_ui;
}
