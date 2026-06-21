#pragma once

#include "navigation/Coordinates.h"
#include "simulation/Runway.h"
#include <string>
#include <vector>

struct Airport {
  std::string icao; // UMMS...
  std::string iata; // MSQ...

  Coordinates position;
  std::vector<Runway> runways;
};
