#pragma once

#include "navigation/WorldPosition.h"
namespace Sim {
class Aircraft {
public:
  Aircraft();
  ~Aircraft();

  void tick(double dt);

private:
  WorldPosition m_position;
  double m_vspeed;
};
} // namespace Sim
