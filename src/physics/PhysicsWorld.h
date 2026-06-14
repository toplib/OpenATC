#pragma once

#include "PhysicsModel.h"
#include <memory>
#include <vector>

struct PhysicsWorld {
  std::vector<std::unique_ptr<Physics::PhysicsModel>> physicsModels;
};
