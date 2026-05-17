#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Settings; }
QT_END_NAMESPACE

class Settings : public QDialog {
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private:
    Ui::Settings* m_ui;
};