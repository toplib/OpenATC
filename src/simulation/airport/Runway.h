#pragma once

#include <string>

struct Runway {
  std::string airportIcao;
  std::string name; // 06R, 27L etc..

  double latitude;
  double longitude;
  float heading;
  bool occupied;
};
