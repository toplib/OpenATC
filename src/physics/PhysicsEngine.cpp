#include "PhysicsEngine.h"
#include "PhysicsModel.h"
#include "PhysicsWorld.h"

namespace Physics {
  PhysicsEngine::PhysicsEngine(PhysicsWorld world) : m_world(std::move(world)) {
    
  }

  PhysicsEngine::~PhysicsEngine() = default;

  void PhysicsEngine::tick(float dt) {
    for (const auto &physicsModel : m_world.physicsModels) {
      physicsModel->tick(dt);
    }
  }
}
