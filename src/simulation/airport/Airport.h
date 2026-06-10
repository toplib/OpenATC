#pragma once
#include "simulation/airport/Runway.h"
#include <string>
#include <vector>

namespace Simulation {
struct AirportConfig {
  std::string icao;

  double latitude;
  double longitude;
};

class Airport {
public:
  explicit Airport(AirportConfig config);
  ~Airport();

private:
  AirportConfig m_config;

  std::vector<Runway> runways;
};
} // namespace Simulation
