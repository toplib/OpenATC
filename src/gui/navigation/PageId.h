#pragma once

#include <QString>

namespace Gui {

enum class PageId {
  Activity,
  AtcMode,
  PilotMode,
  ModelHub,
  Settings,
  Documentation,
  GetStarted,
};

inline QString pageTitle(PageId id) {
  switch (id) {
  case PageId::Activity:
    return QStringLiteral("Activity");
  case PageId::AtcMode:
    return QStringLiteral("ATC Mode");
  case PageId::PilotMode:
    return QStringLiteral("Pilot Mode");
  case PageId::ModelHub:
    return QStringLiteral("Model Hub");
  case PageId::Settings:
    return QStringLiteral("Settings");
  case PageId::Documentation:
    return QStringLiteral("Documentation");
  case PageId::GetStarted:
    return QStringLiteral("Get Started");
  }
  return {};
}

inline PageId pageIdFromNavKey(const QString &key) {
  if (key == QLatin1String("activity"))
    return PageId::Activity;
  if (key == QLatin1String("atc"))
    return PageId::AtcMode;
  if (key == QLatin1String("pilot"))
    return PageId::PilotMode;
  if (key == QLatin1String("models"))
    return PageId::ModelHub;
  if (key == QLatin1String("settings"))
    return PageId::Settings;
  if (key == QLatin1String("docs"))
    return PageId::Documentation;
  if (key == QLatin1String("started"))
    return PageId::GetStarted;
  return PageId::Activity;
}

} // namespace Gui
