#include "Aircraft.h"
#include <iostream>

namespace Simulation {
  Aircraft::Aircraft() = default;
  Aircraft::~Aircraft() = default;

  void Aircraft::tick(float dt) {
    std::cout << "Aircraft::tick() applying force to aircraft" << std::endl;
  }
}
