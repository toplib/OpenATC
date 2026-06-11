#pragma once

#include "navigation/Airway.h"
#include "navigation/Coordinates.h"
#include "navigation/Point.h"
#include <vector>

namespace Navigation {
class IAIRAC {
public:
  ~IAIRAC();

  virtual std::vector<Point> fetchAllPointsByCoordinates(Coordinates start,
                                                         Coordinates end);
  virtual std::vector<Point> fetchAllPoints();
  virtual Point fetchPoint(std::string identifier);

  virtual Airway fetchAirway(std::string identifier);
  virtual std::vector<Airway> fetchAirwaysByCoordinates(Coordinates start,
                                                        Coordinates end);
};
} // namespace Navigation
