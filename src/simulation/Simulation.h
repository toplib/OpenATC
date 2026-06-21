#pragma once

#include "aircraft/Aircraft.h"
#include <vector>
namespace Sim {
class Simulation {
public:
  Simulation();
  ~Simulation();

  void tick(double dt);

private:
  std::vector<Aircraft> m_aircrafts;
};
} // namespace Sim
