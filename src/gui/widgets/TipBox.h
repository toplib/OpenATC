#pragma once

#include <QFrame>
#include <QString>

class QLabel;

namespace Gui {

class TipBox : public QFrame {
  Q_OBJECT

public:
  explicit TipBox(QWidget *parent = nullptr);

  void setTip(const QString &title, const QString &body);

signals:
  void dismissed();

private:
  QLabel *m_title = nullptr;
  QLabel *m_body = nullptr;
};

} // namespace Gui
