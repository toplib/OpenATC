#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include "navigation/Coordinates.h"
#include "navigation/SID.h"
#include "navigation/STAR.h"
#include "simulation/Waypoint.h"

namespace Sim {

struct FlightPlanLeg {
  std::string type;
  std::string fixFrom;
  std::string fixTo;
  Coordinates startPos;
  Coordinates endPos;
  double altitude;
  double speed;
  std::optional<double> altitudeConstraint;
  std::optional<double> speedConstraint;
};

enum class FlightRules {
  IFR,
  VFR,
  SVFR,
};

struct FlightPlan {
  std::string callsign;
  std::string aircraftType;
  FlightRules rules;

  std::string departureAirport;
  std::string arrivalAirport;
  std::string alternateAirport;

  std::string routeString;

  std::string sidName;
  std::string sidTransition;
  std::string starName;
  std::string starTransition;
  std::vector<std::string> airwayRoute;

  std::vector<SIDWaypoint> sidWaypoints;
  std::vector<STARWaypoint> starWaypoints;

  std::vector<FlightPlanLeg> legs;

  std::vector<Waypoint> waypoints;

  std::chrono::system_clock::time_point departureTime;
  double estimatedEnrouteMinutes;
  double fuelMinutes;

  double requestedAltitude;
  double assignedAltitude;
  std::optional<double> initialClimbAltitude;

  int squawkCode;
};

} // namespace Sim
