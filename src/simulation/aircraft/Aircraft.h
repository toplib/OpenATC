#pragma once
#include "simulation/Coordinates.h"
#include "simulation/Entity.h"

namespace Simulation {
class Aircraft : public Entity {
public:
  Aircraft();
  ~Aircraft() override;

private:
  double m_frequency;
  Coordinates m_position;
  double m_speed; // knots
};
} // namespace Simulation
