#include "ContentPages.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Gui {

ContentPage::ContentPage(PageId id, const QString &title, const QString &body,
                         QWidget *parent)
    : IPage(parent), m_id(id) {
  setProperty("class", QStringLiteral("pageRoot"));
  setAttribute(Qt::WA_StyledBackground, true);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(32, 32, 32, 32);
  root->setSpacing(12);

  auto *h = new QLabel(title, this);
  h->setProperty("class", QStringLiteral("pageTitle"));
  root->addWidget(h);

  auto *b = new QLabel(body, this);
  b->setProperty("class", QStringLiteral("pageBody"));
  b->setWordWrap(true);
  b->setMaximumWidth(720);
  root->addWidget(b);

  m_primary = new QPushButton(this);
  m_primary->setProperty("class", QStringLiteral("primaryAction"));
  m_primary->setCursor(Qt::PointingHandCursor);
  m_primary->hide();
  connect(m_primary, &QPushButton::clicked, this,
          &ContentPage::primaryActionTriggered);
  root->addWidget(m_primary, 0, Qt::AlignLeft);

  root->addStretch(1);
}

void ContentPage::setPrimaryAction(const QString &label) {
  m_primary->setText(label);
  m_primary->show();
}

AtcModePage::AtcModePage(QWidget *parent)
    : ContentPage(
          PageId::AtcMode, QStringLiteral("ATC Mode"),
          QStringLiteral(
              "Take the controller seat. Manage arrivals, departures, and "
              "ground movement with live radio, STT, and LLM-assisted clearances."),
          parent) {
  setPrimaryAction(QStringLiteral("Launch ATC session"));
}

PilotModePage::PilotModePage(QWidget *parent)
    : ContentPage(
          PageId::PilotMode, QStringLiteral("Pilot Mode"),
          QStringLiteral(
              "Fly as the pilot. Practice phraseology, listen for clearances, "
              "and respond under realistic radio conditions."),
          parent) {
  setPrimaryAction(QStringLiteral("Launch Pilot session"));
}

ModelHubPage::ModelHubPage(QWidget *parent)
    : ContentPage(
          PageId::ModelHub, QStringLiteral("Model Hub"),
          QStringLiteral(
              "Browse, download, and configure Whisper, Llama, and Sherpa-ONNX "
              "models used by OpenATC backends."),
          parent) {
  setPrimaryAction(QStringLiteral("Open model browser"));
}

DocumentationPage::DocumentationPage(QWidget *parent)
    : ContentPage(
          PageId::Documentation, QStringLiteral("Documentation"),
          QStringLiteral(
              "Guides for installing backends, configuring audio, writing "
              "scenarios, and extending OpenATC with plugins."),
          parent) {
  setPrimaryAction(QStringLiteral("Open quickstart guide"));
}

GetStartedPage::GetStartedPage(QWidget *parent)
    : ContentPage(
          PageId::GetStarted, QStringLiteral("Get Started"),
          QStringLiteral(
              "A short tutorial that walks through creating a project, picking "
              "a scenario, and running your first simulation."),
          parent) {
  setPrimaryAction(QStringLiteral("Start tutorial"));
}

} // namespace Gui
