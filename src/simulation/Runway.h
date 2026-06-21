#pragma once

#include "navigation/Coordinates.h"
#include <string>

struct Runway {
  std::string name; // 31R, 13L...

  Coordinates start;
  Coordinates end;
};
