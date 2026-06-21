#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "simulation/Environment.h"
#include "simulation/FlightPlan.h"
#include "simulation/aircraft/Aircraft.h"
#include "simulation/aircraft/AircraftState.h"
#include "simulation/Command.h"
#include "simulation/Request.h"

namespace Sim {

using ExternalRequestCallback = std::function<void(const Request &)>;

enum class SimEventType {
  AIRCRAFT_ADDED,
  AIRCRAFT_REMOVED,
  STATE_CHANGED,
  REQUEST,
  COLLISION_ALERT,
  SEPARATION_ALERT,
  AIRSPACE_VIOLATION,
  TIME_TICK,
};

struct SimEvent {
  SimEventType type;
  std::string callsign;
  std::string data;
};

using EventCallback = std::function<void(const SimEvent &)>;

class Simulation {
public:
  Simulation();
  ~Simulation();

  void tick(double dt);

  void pause();
  void resume();
  bool isPaused() const;
  void setTimeRate(double rate);
  double getTimeRate() const;
  double getSimTime() const;
  double getWallTime() const;

  void addAircraft(const Aircraft &aircraft);
  void removeAircraft(const std::string &callsign);
  void removeAllAircraft();
  void spawnAircraft(const Aircraft &aircraft, double delaySeconds);
  void spawnAircraft(Aircraft &&aircraft, double delaySeconds);

  Aircraft* findAircraft(const std::string &callsign);
  const Aircraft* findAircraft(const std::string &callsign) const;
  std::vector<Aircraft> &getAircraft();
  const std::vector<Aircraft> &getAircraft() const;
  size_t getAircraftCount() const;

  void dispatchCommand(const Command &cmd);

  template<typename T>
  void dispatchCommand(CommandType type, const std::string &callsign, const T &arg) {
    Command cmd;
    cmd.type = type;
    cmd.callsign = callsign;
    if constexpr (std::is_arithmetic_v<T>)
      cmd.arguments = std::to_string(arg);
    else
      cmd.arguments = arg;
    dispatchCommand(cmd);
  }

  void dispatchCommand(CommandType type, const std::string &callsign) {
    dispatchCommand({type, callsign, ""});
  }

  void setRequestCallback(ExternalRequestCallback cb);
  void subscribe(EventCallback cb);
  void unsubscribeAll();

  void loadScenario(const std::string &path);
  void clearScenario();
  bool isScenarioLoaded() const;

  Environment &getEnvironment();
  const Environment &getEnvironment() const;
  void setEnvironment(const Environment &env);

  void assignFlightPlan(const std::string &callsign, const FlightPlan &fp);
  FlightPlan* getFlightPlan(const std::string &callsign);
  const FlightPlan* getFlightPlan(const std::string &callsign) const;

  std::vector<Aircraft*> findAircraftByState(AircraftState state);
  std::vector<Aircraft*> findAircraftInSector(
      double latMin, double latMax,
      double lonMin, double lonMax,
      double altMin, double altMax);
  std::vector<std::pair<Aircraft*, Aircraft*>> findConflicts(double verticalSep, double horizontalSep);

  size_t getTickCount() const;

private:
  void emitEvent(const SimEvent &evt);
  void checkCollisions();
  void processSpawns(double dt);

  std::vector<Aircraft> m_aircrafts;
  std::unordered_map<std::string, size_t> m_callsignIndex;

  ExternalRequestCallback m_requestCallback;
  std::vector<EventCallback> m_eventCallbacks;

  double m_timeRate = 1.0;
  double m_simTime = 0.0;
  double m_wallTime = 0.0;
  bool m_paused = false;
  size_t m_tickCount = 0;

  Environment m_environment;

  bool m_scenarioLoaded = false;

  struct SpawnEntry {
    Aircraft aircraft;
    double delay;
    SpawnEntry(const Aircraft &ac, double d) : aircraft(ac), delay(d) {}
    SpawnEntry(Aircraft &&ac, double d) : aircraft(std::move(ac)), delay(d) {}
  };
  std::vector<SpawnEntry> m_spawnQueue;

  std::unordered_map<std::string, std::unique_ptr<FlightPlan>> m_flightPlans;
};

} // namespace Sim
