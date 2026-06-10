#pragma once

#include "simulation/aircraft/AircraftType.h"
#include <string>
#include <vector>

struct Parking {
  std::string airportIcao;

  double latitude;
  double longitutde;
  float heading;

  std::vector<AircraftType> allowedTypes;
};
