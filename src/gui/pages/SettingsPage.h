#pragma once

#include "gui/navigation/IPage.h"

namespace Gui {

class SettingsPage : public IPage {
  Q_OBJECT

public:
  explicit SettingsPage(QWidget *parent = nullptr);

  PageId pageId() const override { return PageId::Settings; }
};

} // namespace Gui
