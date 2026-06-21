#pragma once

#include <optional>
#include <string>

#include "navigation/Coordinates.h"
#include "simulation/WaypointType.h"

namespace Sim {

struct Waypoint {
  Coordinates coordinates;
  double altitude;
  WaypointType type;

  std::optional<double> speedRestriction;
  std::optional<double> crossingAltitude;
  std::optional<std::string> name;
  std::optional<double> altitudeConstraint;

  bool holdingStandard = true;
  std::optional<double> holdingRadial;
  std::optional<double> holdingLegTime;
  std::optional<double> holdingLegDistance;
  std::optional<double> holdingMaxSpeed;
  std::optional<double> holdingMinAltitude;
};

} // namespace Sim
