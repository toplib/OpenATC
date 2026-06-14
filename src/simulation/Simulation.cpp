#include "Simulation.h"
#include "World.h"
#include <iostream>

namespace Simulation {
  Simulation::Simulation(const World world) : m_world(world), m_physicsEngine(PhysicsWorld{}) {
  }
  Simulation::~Simulation() = default;

  void Simulation::tick(float dt) {
    // TODO: make spdlog usage
    std::cout << "Simulation::tick()" << std::endl;
  }
}
