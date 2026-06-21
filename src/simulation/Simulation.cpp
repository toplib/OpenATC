#include "Simulation.h"
#include "simulation/aircraft/Aircraft.h"
#include <spdlog/spdlog.h>

namespace Sim {
Simulation::Simulation() {
  // TODO: remove mock data
  for (int i = 0; i < 10; i++) {
    Aircraft aircraft;
    m_aircrafts.push_back(aircraft);
  }
}
Simulation::~Simulation() = default;

void Simulation::tick(double dt) {
  spdlog::info(std::format("Simulation::tick({})", dt));

  for (auto &aircraft : m_aircrafts) {
    aircraft.tick(dt);
  }
}
} // namespace Sim
