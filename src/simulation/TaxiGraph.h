#pragma once

#include <cstdint>
#include <vector>

#include "navigation/Coordinates.h"
#include "simulation/Path.h"

namespace Airports {
struct AirportModel;
}

namespace Sim {

struct TaxiGraph {
  std::vector<Coordinates> nodes;
  std::vector<std::vector<std::pair<size_t, double>>> adj;
  std::vector<uint8_t> flags;

  void build(const Airports::AirportModel &airport);

  size_t findClosestNode(const Coordinates &pos) const;

  Path findPath(size_t from, size_t to) const;

  static constexpr uint8_t FLAG_NONE = 0;
  static constexpr uint8_t FLAG_HOLD_SHORT = 1;
  static constexpr uint8_t FLAG_THRESHOLD = 2;
};

} // namespace Sim
