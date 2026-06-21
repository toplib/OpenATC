#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Coordinates.h"

struct SIDWaypoint {
  std::string name;
  Coordinates coordinates;
  std::optional<double> altitudeConstraint;
  std::optional<double> speedConstraint;
  std::optional<double> gradient;
  bool isRunwayTransition = false;
};

struct SIDTransition {
  std::string name;
  std::vector<SIDWaypoint> waypoints;
};

struct SID {
  std::string name;
  std::vector<std::string> applicableRunways;

  SIDWaypoint initialClimb;
  double initialAltitude;
  double initialSpeed;

  std::vector<SIDTransition> runwayTransitions;
  std::vector<SIDTransition> enrouteTransitions;
  std::vector<SIDWaypoint> allWaypoints;
};
