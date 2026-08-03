#pragma once

#include "PageId.h"

#include <QWidget>

namespace Gui {

class IPage : public QWidget {
  Q_OBJECT

public:
  explicit IPage(QWidget *parent = nullptr) : QWidget(parent) {}
  ~IPage() override = default;

  virtual PageId pageId() const = 0;
  virtual void onActivated() {}
  virtual void onDeactivated() {}
};

} // namespace Gui
