#include "StatusFooter.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QVector>

namespace Gui {

StatusFooter::StatusFooter(QWidget *parent) : QFrame(parent) {
  setObjectName(QStringLiteral("statusFooter"));
  setAttribute(Qt::WA_StyledBackground, true);
  setFixedHeight(24);

  auto *root = new QHBoxLayout(this);
  root->setContentsMargins(8, 0, 10, 0);
  root->setSpacing(0);

  auto *statusKey = new QLabel(QStringLiteral("Status: "), this);
  statusKey->setProperty("class", QStringLiteral("footerText"));
  root->addWidget(statusKey);

  m_statusValue = new QLabel(QStringLiteral("Operational"), this);
  m_statusValue->setProperty("class", QStringLiteral("footerOk"));
  root->addWidget(m_statusValue);

  root->addStretch(1);

  m_backends = new QLabel(this);
  m_backends->setProperty("class", QStringLiteral("footerText"));
  m_backends->setTextFormat(Qt::RichText);
  root->addWidget(m_backends);

  root->addStretch(1);

  m_version = new QLabel(QStringLiteral("Version: 1.0 LTS"), this);
  m_version->setProperty("class", QStringLiteral("footerText"));
  root->addWidget(m_version);

  setBackends({
      {QStringLiteral("Whisper.cpp"), true},
      {QStringLiteral("Llama.cpp"), true},
      {QStringLiteral("Sherpa-Onnx"), true},
  });
}

void StatusFooter::setOperational(bool ok) {
  m_statusValue->setText(ok ? QStringLiteral("Operational")
                            : QStringLiteral("Degraded"));
  m_statusValue->setProperty("class",
                             ok ? QStringLiteral("footerOk")
                                : QStringLiteral("footerText"));
  m_statusValue->style()->unpolish(m_statusValue);
  m_statusValue->style()->polish(m_statusValue);
}

void StatusFooter::setBackends(const QVector<BackendStatus> &backends) {
  m_backendStates = backends;
  refreshBackendLabel();
}

void StatusFooter::setVersion(const QString &version) {
  m_version->setText(QStringLiteral("Version: %1").arg(version));
}

void StatusFooter::refreshBackendLabel() {
  QStringList parts;
  for (const BackendStatus &b : m_backendStates) {
    const QString state =
        b.running ? QStringLiteral("<span style='color:#00FF41;'>(running)</span>")
                  : QStringLiteral("<span style='color:#666666;'>(stopped)</span>");
    parts << QStringLiteral("%1 %2").arg(b.name, state);
  }
  m_backends->setText(QStringLiteral("Backends: %1").arg(parts.join(QStringLiteral(", "))));
}

} // namespace Gui
