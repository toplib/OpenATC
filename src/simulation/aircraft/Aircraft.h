#pragma once
#include "simulation/Entity.h"

namespace Simulation {
class Aircraft : public Entity {
public:
  Aircraft();
  ~Aircraft() override;

private:
  double m_speed; // knots
};
} // namespace Simulation
