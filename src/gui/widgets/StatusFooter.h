#pragma once

#include <QFrame>
#include <QString>

#include <QVector>

class QLabel;

namespace Gui {

struct BackendStatus {
  QString name;
  bool running = false;
};

class StatusFooter : public QFrame {
  Q_OBJECT

public:
  explicit StatusFooter(QWidget *parent = nullptr);

  void setOperational(bool ok);
  void setBackends(const QVector<BackendStatus> &backends);
  void setVersion(const QString &version);

private:
  void refreshBackendLabel();

  QLabel *m_statusValue = nullptr;
  QLabel *m_backends = nullptr;
  QLabel *m_version = nullptr;
  QVector<BackendStatus> m_backendStates;
};

} // namespace Gui
