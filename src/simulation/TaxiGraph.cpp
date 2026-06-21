#include "TaxiGraph.h"

#include <cmath>
#include <cstdint>

#include "Constants.h"
#include "navigation/NavUtils.h"
#include "utils/Algorithms.h"

namespace Sim {

static constexpr double MATCH_TOL = 0.0001;

void TaxiGraph::build(const Airports::AirportModel &airport) {
  nodes.clear();
  adj.clear();
  flags.clear();

  auto getOrCreate = [&](const Coordinates &c) -> size_t {
    for (size_t i = 0; i < nodes.size(); ++i)
      if (nodes[i].latitude == c.latitude && nodes[i].longitude == c.longitude)
        return i;
    nodes.push_back(c);
    adj.push_back({});
    flags.push_back(FLAG_NONE);
    return nodes.size() - 1;
  };

  for (const auto &tw : airport.taxiways) {
    for (size_t i = 0; i + 1 < tw.points.size(); ++i) {
      size_t a = getOrCreate(tw.points[i]);
      size_t b = getOrCreate(tw.points[i + 1]);
      double d = equirectDistance(tw.points[i], tw.points[i + 1]);
      adj[a].push_back({b, d});
      adj[b].push_back({a, d});
    }
  }

  for (const auto &rwy : airport.runways) {
    for (size_t i = 0; i < nodes.size(); ++i) {
      if (std::abs(nodes[i].latitude - rwy.holdShort.latitude) < MATCH_TOL &&
          std::abs(nodes[i].longitude - rwy.holdShort.longitude) < MATCH_TOL)
        flags[i] |= FLAG_HOLD_SHORT;
      if (std::abs(nodes[i].latitude - rwy.threshold.latitude) < MATCH_TOL &&
          std::abs(nodes[i].longitude - rwy.threshold.longitude) < MATCH_TOL)
        flags[i] |= FLAG_THRESHOLD;
    }
  }
}

size_t TaxiGraph::findClosestNode(const Coordinates &pos) const {
  size_t best = 0;
  double bestD = equirectDistance(pos, nodes[0]);
  for (size_t i = 1; i < nodes.size(); ++i) {
    double d = equirectDistance(pos, nodes[i]);
    if (d < bestD) {
      bestD = d;
      best = i;
    }
  }
  return best;
}

Path TaxiGraph::findPath(size_t from, size_t to) const {
  auto heuristic = [&](size_t n) -> double {
    return equirectDistance(nodes[n], nodes[to]);
  };
  auto neighbors = [&](size_t n, std::vector<std::pair<size_t, double>> &out) {
    out = adj[n];
  };

  auto result = Util::aStar<size_t, double>(from, to, heuristic, neighbors);

  Path path;
  if (!result.found)
    return path;

  for (size_t i = 0; i < result.path.size(); ++i) {
    size_t idx = result.path[i];
    uint8_t f = flags[idx];
    WaypointType t = WaypointType::NORMAL;
    if (f & FLAG_THRESHOLD)
      t = WaypointType::RUNWAY_THRESHOLD;
    else if (f & FLAG_HOLD_SHORT)
      t = WaypointType::HOLD_SHORT;
    path.waypoints.push_back({nodes[idx], 0.0, t});
  }

  return path;
}

} // namespace Sim
