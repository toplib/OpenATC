#include "Simulation.h"

#include "navigation/NavUtils.h"

namespace Sim {

Simulation::Simulation() = default;
Simulation::~Simulation() = default;

void Simulation::tick(double dt) {
  if (m_paused) return;

  double simDt = dt * m_timeRate;
  m_simTime += simDt;
  m_wallTime += dt;
  ++m_tickCount;

  processSpawns(dt);

  m_environment.tick(simDt);

  for (auto &aircraft : m_aircrafts)
    aircraft.tick(simDt);

  emitEvent({SimEventType::TIME_TICK, "", std::to_string(m_simTime)});

  checkCollisions();
}

void Simulation::pause() { m_paused = true; }
void Simulation::resume() { m_paused = false; }
bool Simulation::isPaused() const { return m_paused; }
void Simulation::setTimeRate(double rate) { m_timeRate = std::max(0.0, rate); }
double Simulation::getTimeRate() const { return m_timeRate; }
double Simulation::getSimTime() const { return m_simTime; }
double Simulation::getWallTime() const { return m_wallTime; }

void Simulation::addAircraft(const Aircraft &aircraft) {
  auto it = m_callsignIndex.find(aircraft.getCallsign());
  if (it != m_callsignIndex.end()) {
    m_aircrafts[it->second] = aircraft;
  } else {
    m_callsignIndex[aircraft.getCallsign()] = m_aircrafts.size();
    m_aircrafts.push_back(aircraft);
  }

  auto &ac = m_aircrafts[m_callsignIndex[aircraft.getCallsign()]];
  ac.setRequestCallback([this](const Request &r) {
    if (m_requestCallback)
      m_requestCallback(r);
    SimEvent evt;
    evt.type = SimEventType::REQUEST;
    evt.callsign = r.callsign;
    evt.data = std::to_string(static_cast<int>(r.type));
    emitEvent(evt);
  });

  emitEvent({SimEventType::AIRCRAFT_ADDED, aircraft.getCallsign(), ""});
}

void Simulation::removeAircraft(const std::string &callsign) {
  auto it = m_callsignIndex.find(callsign);
  if (it == m_callsignIndex.end()) return;

  size_t idx = it->second;
  m_aircrafts.erase(m_aircrafts.begin() + idx);
  m_callsignIndex.erase(it);

  for (size_t i = idx; i < m_aircrafts.size(); ++i)
    m_callsignIndex[m_aircrafts[i].getCallsign()] = i;

  m_flightPlans.erase(callsign);
  emitEvent({SimEventType::AIRCRAFT_REMOVED, callsign, ""});
}

void Simulation::removeAllAircraft() {
  m_aircrafts.clear();
  m_callsignIndex.clear();
  m_flightPlans.clear();
}

void Simulation::spawnAircraft(const Aircraft &aircraft, double delaySeconds) {
  m_spawnQueue.emplace_back(aircraft, delaySeconds);
}

void Simulation::spawnAircraft(Aircraft &&aircraft, double delaySeconds) {
  m_spawnQueue.emplace_back(std::move(aircraft), delaySeconds);
}

Aircraft* Simulation::findAircraft(const std::string &callsign) {
  auto it = m_callsignIndex.find(callsign);
  if (it == m_callsignIndex.end()) return nullptr;
  return &m_aircrafts[it->second];
}

const Aircraft* Simulation::findAircraft(const std::string &callsign) const {
  auto it = m_callsignIndex.find(callsign);
  if (it == m_callsignIndex.end()) return nullptr;
  return &m_aircrafts[it->second];
}

std::vector<Aircraft> &Simulation::getAircraft() { return m_aircrafts; }
const std::vector<Aircraft> &Simulation::getAircraft() const { return m_aircrafts; }
size_t Simulation::getAircraftCount() const { return m_aircrafts.size(); }

void Simulation::dispatchCommand(const Command &cmd) {
  Aircraft *ac = findAircraft(cmd.callsign);
  if (!ac) return;
  ac->executeCommand(cmd);
}

void Simulation::setRequestCallback(ExternalRequestCallback cb) {
  m_requestCallback = cb;
}

void Simulation::subscribe(EventCallback cb) {
  if (cb)
    m_eventCallbacks.push_back(cb);
}

void Simulation::unsubscribeAll() {
  m_eventCallbacks.clear();
}

void Simulation::emitEvent(const SimEvent &evt) {
  for (auto &cb : m_eventCallbacks)
    cb(evt);
}

void Simulation::loadScenario(const std::string &path) {
  m_scenarioLoaded = true;
}

void Simulation::clearScenario() {
  removeAllAircraft();
  m_scenarioLoaded = false;
}

bool Simulation::isScenarioLoaded() const { return m_scenarioLoaded; }

Environment &Simulation::getEnvironment() { return m_environment; }
const Environment &Simulation::getEnvironment() const { return m_environment; }
void Simulation::setEnvironment(const Environment &env) { m_environment = env; }

void Simulation::assignFlightPlan(const std::string &callsign, const FlightPlan &fp) {
  auto *ac = findAircraft(callsign);
  if (!ac) return;
  ac->setFlightPlan(fp);
  m_flightPlans[callsign] = std::make_unique<FlightPlan>(fp);
}

FlightPlan* Simulation::getFlightPlan(const std::string &callsign) {
  auto it = m_flightPlans.find(callsign);
  if (it == m_flightPlans.end()) return nullptr;
  return it->second.get();
}

const FlightPlan* Simulation::getFlightPlan(const std::string &callsign) const {
  auto it = m_flightPlans.find(callsign);
  if (it == m_flightPlans.end()) return nullptr;
  return it->second.get();
}

std::vector<Aircraft*> Simulation::findAircraftByState(AircraftState state) {
  std::vector<Aircraft*> result;
  for (auto &ac : m_aircrafts) {
    if (ac.getState() == state)
      result.push_back(&ac);
  }
  return result;
}

std::vector<Aircraft*> Simulation::findAircraftInSector(
    double latMin, double latMax,
    double lonMin, double lonMax,
    double altMin, double altMax)
{
  std::vector<Aircraft*> result;
  for (auto &ac : m_aircrafts) {
    const auto &pos = ac.getPosition();
    if (pos.coordinates.latitude >= latMin && pos.coordinates.latitude <= latMax &&
        pos.coordinates.longitude >= lonMin && pos.coordinates.longitude <= lonMax &&
        pos.altitude >= altMin && pos.altitude <= altMax) {
      result.push_back(&ac);
    }
  }
  return result;
}

std::vector<std::pair<Aircraft*, Aircraft*>> Simulation::findConflicts(
    double verticalSep, double horizontalSep)
{
  std::vector<std::pair<Aircraft*, Aircraft*>> conflicts;
  for (size_t i = 0; i < m_aircrafts.size(); ++i) {
    for (size_t j = i + 1; j < m_aircrafts.size(); ++j) {
      auto &a = m_aircrafts[i];
      auto &b = m_aircrafts[j];
      double altDiff = std::abs(a.getPosition().altitude - b.getPosition().altitude);
      if (altDiff > verticalSep) continue;
      double dist = equirectDistance(a.getPosition().coordinates, b.getPosition().coordinates);
      if (dist < horizontalSep * 1852.0) {
        conflicts.emplace_back(&a, &b);
      }
    }
  }
  return conflicts;
}

size_t Simulation::getTickCount() const { return m_tickCount; }

void Simulation::checkCollisions() {
  auto conflicts = findConflicts(1000.0, 5.0);
  for (auto &[a, b] : conflicts) {
    SimEvent evt;
    evt.type = SimEventType::SEPARATION_ALERT;
    evt.callsign = a->getCallsign() + "/" + b->getCallsign();
    emitEvent(evt);
  }
}

void Simulation::processSpawns(double dt) {
  for (auto it = m_spawnQueue.begin(); it != m_spawnQueue.end();) {
    it->delay -= dt;
    if (it->delay <= 0.0) {
      addAircraft(it->aircraft);
      it = m_spawnQueue.erase(it);
    } else {
      ++it;
    }
  }
}

} // namespace Sim
