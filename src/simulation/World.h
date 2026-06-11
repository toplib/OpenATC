#pragma once

#include "simulation/aircraft/Aircraft.h"
#include "simulation/airport/Airport.h"

namespace Simulation {
class World {
public:
  World(Airport airport);
  ~World();

  void addAircraft(Aircraft aircraft);
};
} // namespace Simulation
