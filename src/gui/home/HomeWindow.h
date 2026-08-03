#pragma once

#include "gui/shell/AppShell.h"

/** Compatibility wrapper around the modular AppShell. */
class HomeWindow : public Gui::AppShell {
  Q_OBJECT

public:
  explicit HomeWindow(QWidget *parent = nullptr) : Gui::AppShell(parent) {}
};
