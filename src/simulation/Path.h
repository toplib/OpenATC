#pragma once

#include "simulation/Waypoint.h"
#include <vector>

namespace Sim {

struct Path {
  std::vector<Waypoint> waypoints;
};

} // namespace Sim
