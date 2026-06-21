#pragma once

#include <cmath>
#include <optional>

#include "Coordinates.h"

enum class HoldingEntryType {
  DIRECT,
  PARALLEL,
  TEARDROP,
  UNDETERMINED,
};

struct HoldingPattern {
  Coordinates fix{};
  double inboundRadial = 0.0;
  bool standard = true;
  double legTime = 60.0;
  double minAltitude = 0.0;
  double maxSpeed = 230.0;
  double outboundHeading = 0.0;
  double inboundTrack = 0.0;
};

inline HoldingEntryType determineHoldingEntry(
    double aircraftHeading,
    double inboundRadial,
    bool standard)
{
  double hdg = std::fmod(aircraftHeading, 360.0);
  if (hdg < 0) hdg += 360.0;

  double radial = std::fmod(inboundRadial, 360.0);
  if (radial < 0) radial += 360.0;

  double hdgDiff = std::fmod(radial - hdg + 540.0, 360.0) - 180.0;

  if (standard) {
    if (hdgDiff <= 0.0 && hdgDiff > -180.0) {
      if (hdgDiff > -110.0)
        return HoldingEntryType::DIRECT;
      else
        return HoldingEntryType::TEARDROP;
    } else {
      return HoldingEntryType::PARALLEL;
    }
  } else {
    if (hdgDiff >= 0.0 && hdgDiff < 180.0) {
      if (hdgDiff <= 110.0)
        return HoldingEntryType::DIRECT;
      else
        return HoldingEntryType::TEARDROP;
    } else {
      return HoldingEntryType::PARALLEL;
    }
  }
}
