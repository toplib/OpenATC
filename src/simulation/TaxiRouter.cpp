#include "TaxiRouter.h"

#include <cmath>
#include <algorithm>

namespace Sim {

static constexpr double MATCH_TOL = 0.0001;

Path buildTaxiRoute(const std::vector<std::string> &taxiways,
                    const std::string &destinationRunway,
                    const Airports::AirportModel &airport) {
  Path path;

  for (size_t i = 0; i < taxiways.size(); ++i) {
    const Airports::TaxiwayData *tw = nullptr;
    for (const auto &t : airport.taxiways) {
      if (t.name == taxiways[i]) {
        tw = &t;
        break;
      }
    }
    if (!tw)
      continue;

    for (const auto &pt : tw->points) {
      Waypoint wp;
      wp.coordinates = pt;
      wp.altitude = 0;
      wp.type = WaypointType::NORMAL;
      path.waypoints.push_back(wp);
    }
  }

  for (const auto &rwy : airport.runways) {
    if (rwy.name == destinationRunway) {
      for (auto &wp : path.waypoints) {
        if (std::abs(wp.coordinates.latitude - rwy.holdShort.latitude) < MATCH_TOL &&
            std::abs(wp.coordinates.longitude - rwy.holdShort.longitude) < MATCH_TOL) {
          wp.type = WaypointType::HOLD_SHORT;
          break;
        }
      }
      Waypoint wp;
      wp.coordinates = rwy.threshold;
      wp.altitude = 0;
      wp.type = WaypointType::RUNWAY_THRESHOLD;
      path.waypoints.push_back(wp);
      break;
    }
  }

  return path;
}

} // namespace Sim
