#pragma once

#include "gui/navigation/IPage.h"

#include <QString>

class QPushButton;

namespace Gui {

/** Shared layout helper for mode / content pages. */
class ContentPage : public IPage {
  Q_OBJECT

public:
  ContentPage(PageId id, const QString &title, const QString &body,
              QWidget *parent = nullptr);

  PageId pageId() const override { return m_id; }

signals:
  void primaryActionTriggered();

protected:
  void setPrimaryAction(const QString &label);

private:
  PageId m_id;
  QPushButton *m_primary = nullptr;
};

class AtcModePage : public ContentPage {
  Q_OBJECT
public:
  explicit AtcModePage(QWidget *parent = nullptr);
};

class PilotModePage : public ContentPage {
  Q_OBJECT
public:
  explicit PilotModePage(QWidget *parent = nullptr);
};

class ModelHubPage : public ContentPage {
  Q_OBJECT
public:
  explicit ModelHubPage(QWidget *parent = nullptr);
};

class DocumentationPage : public ContentPage {
  Q_OBJECT
public:
  explicit DocumentationPage(QWidget *parent = nullptr);
};

class GetStartedPage : public ContentPage {
  Q_OBJECT
public:
  explicit GetStartedPage(QWidget *parent = nullptr);
};

} // namespace Gui
