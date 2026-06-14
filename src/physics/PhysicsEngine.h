#pragma once

#include "physics/PhysicsModel.h"
#include "physics/PhysicsWorld.h"
namespace Physics {
  class PhysicsEngine {
  public:
    PhysicsEngine(PhysicsWorld world);
    ~PhysicsEngine();

    void tick(float dt);

    void addPhysicsModel(const PhysicsModel &model);
    PhysicsWorld &getPhysicsWorld();

  private:
    PhysicsWorld m_world;
  };
}
