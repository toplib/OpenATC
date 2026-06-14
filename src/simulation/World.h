#pragma once

#include "simulation/aircraft/Aircraft.h"
#include "simulation/airport/Airport.h"
#include <vector>

struct World {
  std::vector<Simulation::Aircraft> aircrafts;
  std::vector<Simulation::Airport> airports;
};
