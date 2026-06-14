#pragma once

#include "navigation/Position.h"
#include <vector>

struct Engine {
  double forcePowerPerRpm;
  int rpm;
  double egt;

  Position relativePosition;
};

struct AircraftModel {
  std::vector<Engine> engines;
};
