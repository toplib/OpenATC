#include "Constants.h"
#include "simulation/Simulation.h"
#include "simulation/Environment.h"
#include "simulation/TaxiGraph.h"
#include "simulation/TaxiRouter.h"
#include "simulation/aircraft/Aircraft.h"
#include "simulation/aircraft/AircraftState.h"
#include "simulation/aircraft/AircraftType.h"
#include "navigation/HoldingPattern.h"
#include <spdlog/spdlog.h>

static const char *stateName(int s) {
  switch (static_cast<Sim::AircraftState>(s)) {
  case Sim::AircraftState::INACTIVE:           return "INACTIVE";
  case Sim::AircraftState::PARKED:             return "PARKED";
  case Sim::AircraftState::PUSHBACK:           return "PUSHBACK";
  case Sim::AircraftState::PUSHBACK_COMPLETE:  return "PUSHBACK_COMPLETE";
  case Sim::AircraftState::ENGINE_START:       return "ENGINE_START";
  case Sim::AircraftState::TAXIING:            return "TAXIING";
  case Sim::AircraftState::HOLDING_SHORT:      return "HOLDING_SHORT";
  case Sim::AircraftState::LINE_UP:            return "LINE_UP";
  case Sim::AircraftState::BACK_TAXI:          return "BACK_TAXI";
  case Sim::AircraftState::TAKEOFF_ROLL:       return "TAKEOFF_ROLL";
  case Sim::AircraftState::CLIMBOUT:           return "CLIMBOUT";
  case Sim::AircraftState::CLIMBING:           return "CLIMBING";
  case Sim::AircraftState::CRUISING:           return "CRUISING";
  case Sim::AircraftState::DESCENDING:         return "DESCENDING";
  case Sim::AircraftState::HOLDING_ENTRY:      return "HOLDING_ENTRY";
  case Sim::AircraftState::HOLDING_PATTERN:    return "HOLDING_PATTERN";
  case Sim::AircraftState::ARRIVAL:            return "ARRIVAL";
  case Sim::AircraftState::INITIAL_APPROACH:   return "INITIAL_APPROACH";
  case Sim::AircraftState::VECTORS_TO_FINAL:   return "VECTORS_TO_FINAL";
  case Sim::AircraftState::APPROACH:           return "APPROACH";
  case Sim::AircraftState::FINAL_APPROACH:     return "FINAL_APPROACH";
  case Sim::AircraftState::LANDING_FLARE:      return "LANDING_FLARE";
  case Sim::AircraftState::ROLLING:            return "ROLLING";
  case Sim::AircraftState::VACATING:           return "VACATING";
  case Sim::AircraftState::TOUCH_AND_GO:       return "TOUCH_AND_GO";
  case Sim::AircraftState::STOP_AND_GO:        return "STOP_AND_GO";
  case Sim::AircraftState::LOW_APPROACH:       return "LOW_APPROACH";
  case Sim::AircraftState::CIRCLING_APPROACH:  return "CIRCLING_APPROACH";
  case Sim::AircraftState::GO_AROUND:          return "GO_AROUND";
  case Sim::AircraftState::MISSED_APPROACH:    return "MISSED_APPROACH";
  case Sim::AircraftState::EMERGENCY:          return "EMERGENCY";
  case Sim::AircraftState::EMERGENCY_DESCENT:  return "EMERGENCY_DESCENT";
  case Sim::AircraftState::RADIO_FAILURE:      return "RADIO_FAILURE";
  case Sim::AircraftState::MINIMUM_FUEL:       return "MINIMUM_FUEL";
  }
  return "?";
}

int main() {
  Sim::TaxiGraph graph;
  graph.build(Airports::UMMS);

  Sim::Simulation sim;

  // Set up environment
  auto &env = sim.getEnvironment();
  env.wind = {310.0, 8.0, 12.0, 5.0};
  env.visibility = {10000, 5000};
  env.qnh = 1013.0;
  env.temperature = 15.0;

  spdlog::info("ATIS: {}", env.getAtisMessage("UMMS"));

  // Subscribe to events (future orchestrator hook)
  sim.subscribe([](const Sim::SimEvent &evt) {
    if (evt.type == Sim::SimEventType::REQUEST) {
      spdlog::info("Event type={} callsign={}", evt.data, evt.callsign);
    }
  });

  sim.setRequestCallback([](const Sim::Request &req) {
    spdlog::info("Request {} {}", static_cast<int>(req.type), req.callsign);
  });

  // Create aircraft with a type
  {
    Sim::Aircraft a;
    a.setCallsign("BRU7581");
    a.setPosition({Airports::UMMS.gates[0].position, 0});
    a.setState(Sim::AircraftState::PARKED);
    a.setAircraftType(Sim::AircraftDatabase::B738);
    sim.addAircraft(a);
  }

  auto *ac = sim.findAircraft("BRU7581");

  spdlog::info("{} ({}) at {} type={}",
               ac->getCallsign(),
               ac->getAircraftType() ? ac->getAircraftType()->icao : "?",
               stateName(static_cast<int>(ac->getState())),
               ac->getAircraftType() ? ac->getAircraftType()->manufacturer : "?");

  // Test ground ops
  sim.dispatchCommand(Sim::CommandType::PUSHBACK, "BRU7581");
  sim.tick(6.0);

  sim.dispatchCommand(Sim::CommandType::START_ENGINES, "BRU7581");
  sim.tick(1.0);
  sim.dispatchCommand(Sim::CommandType::START_TAXI, "BRU7581");

  // Build and assign a taxi route
  size_t start = graph.findClosestNode(Airports::UMMS.gates[0].position);
  size_t goal = graph.findClosestNode(Airports::UMMS.runways[0].threshold);
  auto route = graph.findPath(start, goal);
  if (route.waypoints.empty()) {
    spdlog::error("No path from gate to runway");
    return 1;
  }
  route.waypoints.back().type = Sim::WaypointType::RUNWAY_THRESHOLD;

  ac = sim.findAircraft("BRU7581");
  ac->setPath(route);

  sim.dispatchCommand(Sim::CommandType::LINE_UP_AND_WAIT, "BRU7581");
  sim.dispatchCommand(Sim::CommandType::CLEARED_TAKEOFF, "BRU7581");

  spdlog::info("Starting taxi from ({:.4f}, {:.4f}) hdg={:.0f}",
               ac->getPosition().coordinates.latitude,
               ac->getPosition().coordinates.longitude,
               ac->getHeading());

  // Run simulation for 200 seconds
  int prevState = -1;
  for (int step = 0; step < 200; ++step) {
    sim.tick(1.0);

    int cur = static_cast<int>(ac->getState());
    if (cur != prevState) {
      auto pos = ac->getPosition();
      spdlog::info("{}: {} -> {} t={} lat={:.4f} lon={:.4f} alt={:.0f} hdg={:.0f} spd={:.0f} wp={}",
                   ac->getCallsign(),
                   prevState >= 0 ? stateName(prevState) : "-",
                   stateName(cur), step, pos.coordinates.latitude,
                   pos.coordinates.longitude, pos.altitude,
                   ac->getHeading(), ac->getSpeed(), (int)ac->getWaypointIndex());
      prevState = cur;
    }
  }

  // Test holding pattern
  spdlog::info("--- Testing holding pattern ---");
  ac->setPosition({{53.900, 28.050}, 5000});
  ac->setState(Sim::AircraftState::CRUISING);
  ac->setSpeed(220);

  HoldingPattern hp;
  hp.fix = {53.905, 28.040};
  hp.inboundRadial = 310.0;
  hp.standard = true;
  hp.legTime = 60.0;
  hp.minAltitude = 5000.0;
  hp.maxSpeed = 230.0;

  ac->assignHoldingPattern(hp);

  prevState = -1;
  for (int step = 0; step < 300; ++step) {
    sim.tick(1.0);

    int cur = static_cast<int>(ac->getState());
    if (cur != prevState) {
      auto pos = ac->getPosition();
      spdlog::info("{}: {} -> {} t={} lat={:.4f} lon={:.4f} alt={:.0f} hdg={:.0f} spd={:.0f}",
                   ac->getCallsign(),
                   prevState >= 0 ? stateName(prevState) : "-",
                   stateName(cur), step, pos.coordinates.latitude,
                   pos.coordinates.longitude, pos.altitude,
                   ac->getHeading(), ac->getSpeed());
      prevState = cur;
    }
  }

  // Test go-around
  spdlog::info("--- Testing go-around ---");
  ac->setPosition({{53.890, 28.030}, 300});
  ac->setSpeed(140);
  ac->setState(Sim::AircraftState::APPROACH);

  sim.dispatchCommand(Sim::CommandType::GO_AROUND, "BRU7581");

  prevState = -1;
  for (int step = 0; step < 100; ++step) {
    sim.tick(1.0);

    int cur = static_cast<int>(ac->getState());
    if (cur != prevState) {
      auto pos = ac->getPosition();
      spdlog::info("{}: {} -> {} t={} lat={:.4f} lon={:.4f} alt={:.0f} hdg={:.0f} spd={:.0f}",
                   ac->getCallsign(),
                   prevState >= 0 ? stateName(prevState) : "-",
                   stateName(cur), step, pos.coordinates.latitude,
                   pos.coordinates.longitude, pos.altitude,
                   ac->getHeading(), ac->getSpeed());
      prevState = cur;
    }
  }

  spdlog::info("Simulation complete. Sim time={:.0f}s, ticks={}",
               sim.getSimTime(), sim.getTickCount());
  return 0;
}
