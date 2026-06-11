#pragma once

#include "Point.h"
#include <string>
#include <vector>

struct Airway {
  std::string identifier;
  std::vector<Point> points;
};
