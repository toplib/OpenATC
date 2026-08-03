#pragma once

#include "gui/navigation/IPage.h"
#include "gui/navigation/PageId.h"

namespace Gui {

class ActivityPage : public IPage {
  Q_OBJECT

public:
  explicit ActivityPage(QWidget *parent = nullptr);

  PageId pageId() const override { return PageId::Activity; }

signals:
  void simulateRequested();
  void marketplaceRequested();
  void tutorialRequested();
  void aboutRequested();
};

} // namespace Gui
