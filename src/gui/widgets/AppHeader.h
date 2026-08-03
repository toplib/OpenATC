#pragma once

#include "gui/navigation/PageId.h"

#include <QFrame>
#include <QHash>
#include <QHBoxLayout>
#include <QPushButton>

class QLineEdit;

namespace Gui {

class AppHeader : public QFrame {
  Q_OBJECT

public:
  explicit AppHeader(QWidget *parent = nullptr);

  void setActivePage(PageId id);
  QString searchText() const;
  void clearSearch();

signals:
  void pageRequested(PageId id);
  void searchSubmitted(const QString &query);
  void avatarClicked();

private:
  void addNav(const QString &label, PageId id, QHBoxLayout *layout);

  QHash<PageId, QPushButton *> m_navButtons;
  QLineEdit *m_search = nullptr;
};

} // namespace Gui
