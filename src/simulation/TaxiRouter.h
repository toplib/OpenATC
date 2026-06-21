#pragma once

#include <string>
#include <vector>

#include "Constants.h"
#include "simulation/Path.h"
#include "simulation/Waypoint.h"

namespace Sim {

Path buildTaxiRoute(const std::vector<std::string> &taxiways,
                    const std::string &destinationRunway,
                    const Airports::AirportModel &airport);

} // namespace Sim
