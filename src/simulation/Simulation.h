#pragma once

#include "physics/PhysicsEngine.h"
#include "simulation/World.h"
namespace Simulation {
  class Simulation {
    public:
    Simulation(const World world);
    ~Simulation();

    void tick(float dt);
    private:
    World m_world;
    Physics::PhysicsEngine m_physicsEngine;
  };
}
