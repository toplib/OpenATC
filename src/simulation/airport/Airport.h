#pragma once
#include "simulation/Frequency.h"
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
  std::string m_icao;
  std::string m_iata;

  AirportConfig m_config;

  std::vector<Runway> runways;
  std::vector<Frequency> m_frequencies;
};
} // namespace Simulation
