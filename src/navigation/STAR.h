#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Coordinates.h"

struct STARWaypoint {
  std::string name;
  Coordinates coordinates;
  std::optional<double> altitudeConstraint;
  std::optional<double> speedConstraint;
  bool isRunwayTransition = false;
};

struct STARTransition {
  std::string name;
  std::vector<STARWaypoint> waypoints;
};

struct STAR {
  std::string name;
  std::vector<std::string> applicableRunways;

  std::vector<STARTransition> enrouteTransitions;
  std::vector<STARWaypoint> commonRoute;
  std::vector<STARTransition> runwayTransitions;
  std::vector<STARWaypoint> allWaypoints;
};
