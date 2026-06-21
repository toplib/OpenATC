#include "Aircraft.h"

#include <algorithm>

#include "navigation/NavUtils.h"

namespace Sim {

static constexpr double KNOTS_TO_MS = 0.514444;
static constexpr double TURN_RATE_AIR = 3.0;
static constexpr double TURN_RATE_GROUND = 5.0;
static constexpr double MAX_FOLLOW_ERR_AIR = 90.0;
static constexpr double MAX_FOLLOW_ERR_GROUND = 30.0;
static constexpr double SNAP_DIST = 100.0;
static constexpr double SPEED_RAMP = 5.0;
static constexpr double ALT_P_GAIN = 0.2;
static constexpr double ALT_MAX = 25.0;
static constexpr double TAXI_SPEED = 20.0;
static constexpr double TO_ACCEL = 8.0;
static constexpr double TO_ROTATE = 150.0;
static constexpr double CLIMBOUT_VS = 15.0;
static constexpr double CLIMB_VS = 12.0;
static constexpr double CLIMBOUT_ALT = 3000.0;
static constexpr double CRUISE_ALT = 10000.0;
static constexpr double DESCEND_VS = -10.0;
static constexpr double APPROACH_ALT = 500.0;
static constexpr double FINAL_VS = -5.0;
static constexpr double FINAL_ALT = 50.0;
static constexpr double LANDING_VS = -3.0;
static constexpr double LANDING_ALT = 0.0;
static constexpr double FLARE_DECEL = 0.97;
static constexpr double FLARE_SPD = 30.0;
static constexpr double ROLL_DECEL = 0.9;
static constexpr double ROLL_SPD = 5.0;
static constexpr double VACATE_SPD = 15.0;
static constexpr double GAROUND_VS = 18.0;
static constexpr double GAROUND_SPD = 160.0;
static constexpr double HOLDING_BANK = 25.0;
static constexpr double HOLDING_TURN_RATE = 3.0;
static constexpr double EMERGENCY_DESCENT_VS = -50.0;

Aircraft::Aircraft()
  : m_vspeed(0)
  , m_targetVspeed(0)
  , m_state(AircraftState::PARKED)
  , m_waypointIndex(0)
  , m_speed(0)
  , m_targetSpeed(0)
  , m_heading(0)
  , m_targetHeading(0)
  , m_targetAltitude(0)
  , m_aircraftType(&AircraftDatabase::B738)
  , m_pushbackDist(0)
{}

Aircraft::~Aircraft() = default;

void Aircraft::setState(AircraftState state) { m_state = state; }
AircraftState Aircraft::getState() const { return m_state; }

void Aircraft::setCallsign(const std::string &cs) { m_callsign = cs; }
const std::string &Aircraft::getCallsign() const { return m_callsign; }

void Aircraft::setSpeed(double knots) { m_targetSpeed = knots; }
void Aircraft::setHeading(double hdg) { m_heading = normalizeHeading(hdg); m_targetHeading = m_heading; }

void Aircraft::setPosition(const WorldPosition &pos) { m_position = pos; }
const WorldPosition &Aircraft::getPosition() const { return m_position; }

size_t Aircraft::getWaypointIndex() const { return m_waypointIndex; }
double Aircraft::getSpeed() const { return m_speed; }
double Aircraft::getVSpeed() const { return m_vspeed; }
double Aircraft::getHeading() const { return m_heading; }
double Aircraft::getTargetAltitude() const { return m_targetAltitude; }

void Aircraft::setRequestCallback(RequestCallback cb) { m_requestCb = cb; }
void Aircraft::emitRequest(const Request &r) { if (m_requestCb) m_requestCb(r); }

void Aircraft::setFlightPlan(const FlightPlan &fp) {
  m_flightPlan = fp;
}

const FlightPlan* Aircraft::getFlightPlan() const { return m_flightPlan ? &*m_flightPlan : nullptr; }

void Aircraft::setAircraftType(const AircraftType &type) {
  m_aircraftType = &type;
}

const AircraftType* Aircraft::getAircraftType() const { return m_aircraftType; }

void Aircraft::setSquawk(int code) { m_squawk = code; }
int Aircraft::getSquawk() const { return m_squawk; }

void Aircraft::declareEmergency(const std::string &nature) {
  m_emergency = true;
  m_emergencyNature = nature;
  m_squawk7700 = true;
  m_squawk = 7700;
  if (nature == "RADIO_FAILURE") {
    m_radioFailure = true;
    m_squawk = 7600;
    m_squawk7700 = false;
    m_squawk7600 = true;
  }
  emitRequest({RequestType::MAYDAY_DECLARED, m_callsign, nature});
}

void Aircraft::cancelEmergency() {
  m_emergency = false;
  m_emergencyNature.clear();
  m_squawk7700 = false;
  m_squawk7600 = false;
  m_radioFailure = false;
  m_squawk = 1200;
  emitRequest({RequestType::EMERGENCY_CANCELLED, m_callsign, ""});
}

bool Aircraft::isEmergency() const { return m_emergency; }
std::string Aircraft::getEmergencyNature() const { return m_emergencyNature; }

void Aircraft::setFrequency(double freq) { m_frequency = freq; }
double Aircraft::getFrequency() const { return m_frequency; }
void Aircraft::setController(const std::string &controller) { m_controller = controller; }
std::string Aircraft::getController() const { return m_controller; }

void Aircraft::assignApproach(ApproachType type, const std::string &runway) {
  m_assignedApproach = type;
  m_assignedRunway = runway;
}

Aircraft::ApproachType Aircraft::getAssignedApproach() const { return m_assignedApproach; }
std::string Aircraft::getAssignedRunway() const { return m_assignedRunway; }

void Aircraft::assignHoldingPattern(const HoldingPattern &pattern) {
  m_holdingPattern = pattern;
  m_holdingEntryType = HoldingEntryType::UNDETERMINED;
  m_holdingTimer = 0.0;
  m_holdingLegCount = 0;
  m_holdingTurning = false;
  m_holdingEntryTimer = 0.0;
  m_holdingEntryDone = false;
  m_state = AircraftState::HOLDING_ENTRY;
}

bool Aircraft::isInHolding() const {
  return m_state == AircraftState::HOLDING_ENTRY ||
         m_state == AircraftState::HOLDING_PATTERN;
}

std::optional<HoldingPattern> Aircraft::getHoldingPattern() const {
  return m_holdingPattern;
}

void Aircraft::setTargetAltitude(double alt) { m_targetAltitude = alt; }
std::optional<Command> Aircraft::getLastCommand() const { return m_lastCommand; }

void Aircraft::setPath(const Path &path) {
  m_path = path;
  m_waypointIndex = 0;
  if (!m_path.waypoints.empty()) {
    m_heading = bearing(m_position.coordinates, m_path.waypoints[0].coordinates);
    m_targetHeading = m_heading;
  }
}

void Aircraft::setInitialHeadingToWaypoint() {
  if (!m_path.waypoints.empty()) {
    m_heading = bearing(m_position.coordinates, m_path.waypoints[0].coordinates);
    m_targetHeading = m_heading;
  }
}

bool Aircraft::preconditionMet(const Command &cmd) {
  switch (cmd.type) {
    case CommandType::LINE_UP_AND_WAIT:
      return m_state == AircraftState::HOLDING_SHORT;
    case CommandType::CLEARED_TAKEOFF:
      return m_state == AircraftState::LINE_UP;
    case CommandType::CLEARED_IMMEDIATE_TAKEOFF:
      return m_state == AircraftState::LINE_UP || m_state == AircraftState::TAXIING;
    case CommandType::CLEARED_LAND:
      return m_state == AircraftState::APPROACH ||
             m_state == AircraftState::FINAL_APPROACH;
    default:
      return true;
  }
}

void Aircraft::executeCommand(const Command &cmd) {
  m_lastCommand = cmd;

  if (cmd.type == CommandType::LINE_UP_AND_WAIT) {
    if (m_state == AircraftState::TAXIING || m_state == AircraftState::HOLDING_SHORT) {
      m_queuedCommands.push_back(cmd);
      return;
    }
  }
  if (cmd.type == CommandType::CLEARED_TAKEOFF || cmd.type == CommandType::CLEARED_IMMEDIATE_TAKEOFF) {
    if (m_state == AircraftState::TAXIING || m_state == AircraftState::HOLDING_SHORT ||
        m_state == AircraftState::LINE_UP) {
      m_queuedCommands.push_back(cmd);
      return;
    }
  }

  executeImmediate(cmd);
}

void Aircraft::executeImmediate(const Command &cmd) {
  switch (cmd.type) {
    case CommandType::PUSHBACK:
      if (m_state == AircraftState::PARKED) {
        m_state = AircraftState::PUSHBACK;
        m_targetSpeed = 5.0;
        emitRequest({RequestType::PUSHBACK_START, m_callsign, ""});
      }
      break;

    case CommandType::START_ENGINES:
      if (m_state == AircraftState::PUSHBACK || m_state == AircraftState::PUSHBACK_COMPLETE) {
        m_state = AircraftState::ENGINE_START;
        emitRequest({RequestType::ENGINE_STARTED, m_callsign, ""});
      }
      break;

    case CommandType::START_TAXI:
      if (m_state == AircraftState::PARKED || m_state == AircraftState::PUSHBACK ||
          m_state == AircraftState::PUSHBACK_COMPLETE || m_state == AircraftState::ENGINE_START) {
        m_state = AircraftState::TAXIING;
        m_targetSpeed = TAXI_SPEED;
        emitRequest({RequestType::READY_TO_TAXI, m_callsign, ""});
      }
      break;

    case CommandType::HOLD_SHORT:
      if (m_state == AircraftState::TAXIING)
        m_state = AircraftState::HOLDING_SHORT;
      break;

    case CommandType::LINE_UP_AND_WAIT:
      if (m_state == AircraftState::HOLDING_SHORT)
        m_state = AircraftState::LINE_UP;
      break;

    case CommandType::CROSS_RUNWAY:
      break;

    case CommandType::BACK_TAXI:
      if (m_state == AircraftState::TAXIING || m_state == AircraftState::LINE_UP) {
        m_state = AircraftState::BACK_TAXI;
      }
      break;

    case CommandType::EXPEDITE_TAXI:
      m_targetSpeed = TAXI_SPEED * 1.5;
      break;

    case CommandType::STOP_TAXI:
      if (m_state == AircraftState::TAXIING) {
        m_targetSpeed = 0;
      }
      break;

    case CommandType::SHUTDOWN_ENGINES:
      if (m_state == AircraftState::PARKED) {
        m_state = AircraftState::INACTIVE;
      }
      break;

    case CommandType::CLEARED_TAKEOFF:
    case CommandType::CLEARED_IMMEDIATE_TAKEOFF:
      if (m_state == AircraftState::LINE_UP || m_state == AircraftState::PARKED ||
          m_state == AircraftState::PUSHBACK || m_state == AircraftState::TAXIING ||
          m_state == AircraftState::HOLDING_SHORT) {
        m_state = AircraftState::TAKEOFF_ROLL;
        emitRequest({RequestType::TAKEOFF_ROLL_START, m_callsign, ""});
      }
      break;

    case CommandType::CANCEL_TAKEOFF_CLEARANCE:
      if (m_state == AircraftState::TAKEOFF_ROLL) {
        m_state = AircraftState::TAXIING;
        m_speed = 0;
        m_targetSpeed = 0;
      }
      break;

    case CommandType::CLEARED_FOR_OPTION:
      break;

    case CommandType::SET_HEADING:
    case CommandType::FLY_HEADING:
      m_targetHeading = normalizeHeading(std::stod(cmd.arguments));
      m_path.waypoints.clear();
      m_waypointIndex = 0;
      break;

    case CommandType::TURN_LEFT_HEADING:
      m_targetHeading = normalizeHeading(m_heading - std::stod(cmd.arguments));
      break;

    case CommandType::TURN_RIGHT_HEADING:
      m_targetHeading = normalizeHeading(m_heading + std::stod(cmd.arguments));
      break;

    case CommandType::DIRECT_TO:
      break;

    case CommandType::SET_ALTITUDE:
    case CommandType::CLIMB_TO:
    case CommandType::DESCEND_TO:
      m_targetAltitude = std::stod(cmd.arguments);
      break;

    case CommandType::EXPEDITE_CLIMB:
      m_targetVspeed = m_aircraftType ? m_aircraftType->perf.climbRate / 60.0 * 1.5 : 30.0;
      break;

    case CommandType::EXPEDITE_DESCENT:
      m_targetVspeed = m_aircraftType ? -(m_aircraftType->perf.descentRate / 60.0 * 1.5) : -30.0;
      break;

    case CommandType::SET_SPEED:
    case CommandType::MAINTAIN_SPEED:
      m_targetSpeed = std::stod(cmd.arguments);
      break;

    case CommandType::INCREASE_SPEED: {
      double delta = cmd.arguments.empty() ? 20.0 : std::stod(cmd.arguments);
      m_targetSpeed += delta;
      break;
    }

    case CommandType::REDUCE_SPEED: {
      double delta = cmd.arguments.empty() ? 20.0 : std::stod(cmd.arguments);
      m_targetSpeed = std::max(0.0, m_targetSpeed - delta);
      break;
    }

    case CommandType::SPEED_250_OR_LESS:
      m_targetSpeed = std::min(m_speed, 250.0);
      break;

    case CommandType::CLEARED_APPROACH:
    case CommandType::CLEARED_ILS_APPROACH:
    case CommandType::CLEARED_VISUAL_APPROACH:
    case CommandType::CLEARED_VOR_APPROACH:
    case CommandType::CLEARED_NDB_APPROACH:
    case CommandType::CLEARED_RNAV_APPROACH:
    case CommandType::CLEARED_LOCALIZER_APPROACH:
    case CommandType::CLEARED_CIRCLE_TO_LAND:
      if (m_state == AircraftState::APPROACH || m_state == AircraftState::DESCENDING ||
          m_state == AircraftState::CRUISING) {
        if (m_state == AircraftState::APPROACH)
          m_state = AircraftState::INITIAL_APPROACH;
        else
          m_state = AircraftState::DESCENDING;
        emitRequest({RequestType::APPROACH_CLEARANCE_REQUIRED, m_callsign, cmd.arguments});
      }
      break;

    case CommandType::CLEARED_LAND:
      if (m_state == AircraftState::APPROACH || m_state == AircraftState::FINAL_APPROACH) {
        emitRequest({RequestType::CLEARED_TO_LAND, m_callsign, ""});
      }
      break;

    case CommandType::CLEARED_TOUCH_AND_GO:
      m_state = AircraftState::TOUCH_AND_GO;
      emitRequest({RequestType::TOUCH_AND_GO_INITIATED, m_callsign, ""});
      break;

    case CommandType::CLEARED_STOP_AND_GO:
      m_state = AircraftState::STOP_AND_GO;
      emitRequest({RequestType::STOP_AND_GO_INITIATED, m_callsign, ""});
      break;

    case CommandType::LAHSO:
      break;

    case CommandType::GO_AROUND:
      if (m_state == AircraftState::APPROACH || m_state == AircraftState::FINAL_APPROACH ||
          m_state == AircraftState::INITIAL_APPROACH || m_state == AircraftState::LANDING_FLARE) {
        m_state = AircraftState::GO_AROUND;
        m_targetSpeed = GAROUND_SPD;
        m_targetVspeed = GAROUND_VS;
        m_targetAltitude = CLIMBOUT_ALT;
        m_path.waypoints.clear();
        m_waypointIndex = 0;
        emitRequest({RequestType::GO_AROUND_INITIATED, m_callsign, cmd.arguments});
      }
      break;

    case CommandType::EXECUTE_MISSED_APPROACH:
      if (m_state == AircraftState::APPROACH || m_state == AircraftState::FINAL_APPROACH ||
          m_state == AircraftState::INITIAL_APPROACH) {
        m_state = AircraftState::MISSED_APPROACH;
        m_targetSpeed = GAROUND_SPD;
        m_targetVspeed = GAROUND_VS;
        m_targetAltitude = CLIMBOUT_ALT;
        m_path.waypoints.clear();
        m_waypointIndex = 0;
        emitRequest({RequestType::MISSED_APPROACH_INITIATED, m_callsign, cmd.arguments});
      }
      break;

    case CommandType::HOLD_AT: {
      HoldingPattern hp;
      hp.standard = true;
      hp.legTime = 60.0;
      hp.minAltitude = m_position.altitude;
      hp.maxSpeed = m_aircraftType ? m_aircraftType->perf.maxHoldingSpeed : 230.0;
      assignHoldingPattern(hp);
      break;
    }

    case CommandType::HOLD_AS_PUBLISHED:
    case CommandType::HOLD_STANDARD: {
      HoldingPattern hp;
      hp.standard = true;
      hp.legTime = 60.0;
      hp.minAltitude = m_position.altitude;
      hp.maxSpeed = m_aircraftType ? m_aircraftType->perf.maxHoldingSpeed : 230.0;
      if (m_path.waypoints.size() > m_waypointIndex) {
        hp.fix = m_path.waypoints[m_waypointIndex].coordinates;
      } else {
        hp.fix = m_position.coordinates;
      }
      assignHoldingPattern(hp);
      break;
    }

    case CommandType::HOLD_NONSTANDARD: {
      HoldingPattern hp;
      hp.standard = false;
      hp.legTime = 60.0;
      assignHoldingPattern(hp);
      break;
    }

    case CommandType::CANCEL_HOLD:
      if (isInHolding()) {
        m_holdingPattern.reset();
        m_state = AircraftState::CRUISING;
        emitRequest({RequestType::HOLDING_EXITED, m_callsign, ""});
      }
      break;

    case CommandType::CONTACT:
      if (!cmd.arguments.empty()) {
        m_frequency = std::stod(cmd.arguments);
        m_controller = cmd.data;
        emitRequest({RequestType::FREQUENCY_CHANGED, m_callsign, cmd.arguments});
      }
      break;

    case CommandType::MONITOR:
      if (!cmd.arguments.empty()) {
        m_frequency = std::stod(cmd.arguments);
      }
      break;

    case CommandType::SQUAWK:
      if (!cmd.arguments.empty()) {
        m_squawk = std::stoi(cmd.arguments);
      }
      break;

    case CommandType::SQUAWK_IDENT:
      emitRequest({RequestType::CONTACT_MADE, m_callsign, "IDENT"});
      break;

    case CommandType::SQUAWK_STANDBY:
      m_squawk = 1200;
      break;

    case CommandType::SQUAWK_VFR:
      m_squawk = 1200;
      break;

    case CommandType::SET_ALTIMETER:
      break;

    case CommandType::REPORT_POSITION:
      emitRequest({RequestType::POSITION_REPORT, m_callsign,
                   std::to_string(m_position.coordinates.latitude) + "," +
                   std::to_string(m_position.coordinates.longitude) + "," +
                   std::to_string((int)m_position.altitude)});
      break;

    case CommandType::REPORT_ALTITUDE:
      emitRequest({RequestType::POSITION_REPORT, m_callsign,
                   "ALT " + std::to_string((int)m_position.altitude)});
      break;

    case CommandType::DECLARE_EMERGENCY:
      declareEmergency(cmd.arguments.empty() ? "GENERAL" : cmd.arguments);
      break;

    case CommandType::DECLARE_PAN_PAN:
      m_emergency = true;
      m_emergencyNature = cmd.arguments.empty() ? "URGENCY" : cmd.arguments;
      m_squawk = 7700;
      emitRequest({RequestType::PAN_PAN_DECLARED, m_callsign, cmd.arguments});
      break;

    case CommandType::CANCEL_EMERGENCY:
      cancelEmergency();
      break;

    case CommandType::EMERGENCY_DESCENT:
      m_targetVspeed = EMERGENCY_DESCENT_VS;
      m_state = AircraftState::EMERGENCY_DESCENT;
      emitRequest({RequestType::EMERGENCY_DESCENT_STARTED, m_callsign, ""});
      break;

    case CommandType::MINIMUM_FUEL_ADVISORY:
      emitRequest({RequestType::MINIMUM_FUEL, m_callsign, cmd.arguments});
      break;

    case CommandType::HANDOFF:
      m_controller = cmd.arguments;
      emitRequest({RequestType::HANDOFF_COMPLETE, m_callsign, cmd.arguments});
      break;

    default:
      break;
  }
}

void Aircraft::processCommandQueue() {
  for (auto it = m_queuedCommands.begin(); it != m_queuedCommands.end();) {
    if (preconditionMet(*it)) {
      executeImmediate(*it);
      it = m_queuedCommands.erase(it);
    } else {
      ++it;
    }
  }
}

void Aircraft::flyHeading(double dt) {
  if (std::abs(headingError(m_heading, m_targetHeading)) > 1.0) {
    m_heading = turnToward(m_heading, m_targetHeading, TURN_RATE_AIR, dt);
  }

  double step = m_speed * KNOTS_TO_MS * dt;
  if (step <= 0) return;
  double hdgRad = toRad(m_heading);
  double lat = m_position.coordinates.latitude;
  m_position.coordinates.latitude += step * std::cos(hdgRad) / EARTH_M;
  m_position.coordinates.longitude += step * std::sin(hdgRad)
    / (EARTH_M * std::cos(toRad(lat)));
}

void Aircraft::followPath(double dt) {
  if (m_path.waypoints.empty() || m_waypointIndex >= m_path.waypoints.size()) {
    flyHeading(dt);
    return;
  }

  const auto &target = m_path.waypoints[m_waypointIndex];
  double d = equirectDistance(m_position.coordinates, target.coordinates);

  if (d < SNAP_DIST) {
    m_position.coordinates = target.coordinates;
    m_position.altitude = target.altitude;

    switch (target.type) {
      case WaypointType::HOLD_SHORT:
        m_state = AircraftState::HOLDING_SHORT;
        emitRequest({RequestType::HOLD_SHORT_REACHED, m_callsign, ""});
        break;
      case WaypointType::RUNWAY_THRESHOLD:
        if (m_state == AircraftState::TAXIING || m_state == AircraftState::HOLDING_SHORT
            || m_state == AircraftState::BACK_TAXI || m_state == AircraftState::LINE_UP) {
          m_state = AircraftState::LINE_UP;
          emitRequest({RequestType::LINE_UP_REACHED, m_callsign, ""});
        }
        break;
      case WaypointType::GATE:
      case WaypointType::RAMP:
      case WaypointType::HANGAR:
        m_state = AircraftState::PARKED;
        emitRequest({RequestType::REACHED_GATE, m_callsign, ""});
        break;
      case WaypointType::RUNWAY_EXIT:
        if (m_state == AircraftState::ROLLING || m_state == AircraftState::VACATING) {
          m_state = AircraftState::VACATING;
          emitRequest({RequestType::VACATED_RUNWAY, m_callsign, ""});
        }
        break;
      case WaypointType::MISSED_APPROACH_POINT:
        if (m_state == AircraftState::MISSED_APPROACH) {
          emitRequest({RequestType::MISSED_APPROACH_PUBLISHED, m_callsign, ""});
        }
        break;
      default:
        emitRequest({RequestType::WAYPOINT_CROSSED, m_callsign,
                     std::to_string(m_waypointIndex)});
        break;
    }

    ++m_waypointIndex;
    return;
  }

  double targetBearing = bearing(m_position.coordinates, target.coordinates);
  double err = headingError(m_heading, targetBearing);

  bool onGround = m_state == AircraftState::TAXIING
               || m_state == AircraftState::HOLDING_SHORT
               || m_state == AircraftState::LINE_UP
               || m_state == AircraftState::BACK_TAXI;
  double turnRate = onGround ? TURN_RATE_GROUND : TURN_RATE_AIR;
  double maxFollowErr = onGround ? MAX_FOLLOW_ERR_GROUND : MAX_FOLLOW_ERR_AIR;

  m_heading = turnToward(m_heading, targetBearing, turnRate, dt);

  if (std::abs(err) >= maxFollowErr) return;

  double step = m_speed * KNOTS_TO_MS * dt;
  if (step >= d) {
    m_position.coordinates = target.coordinates;
    m_position.altitude = target.altitude;
    ++m_waypointIndex;
    return;
  }

  double hdgRad = toRad(m_heading);
  double lat = m_position.coordinates.latitude;
  m_position.coordinates.latitude += step * std::cos(hdgRad) / EARTH_M;
  m_position.coordinates.longitude += step * std::sin(hdgRad)
    / (EARTH_M * std::cos(toRad(lat)));
}

void Aircraft::controlSpeed(double dt) {
  if (m_targetSpeed > 0) {
    double diff = m_targetSpeed - m_speed;
    double ramp = SPEED_RAMP;
    if (std::abs(diff) > 1.0)
      m_speed += std::copysign(ramp * dt, diff);
    else
      m_speed = m_targetSpeed;
  }
}

void Aircraft::controlAltitude(double dt) {
  if (m_targetAltitude > 0) {
    double altDiff = m_targetAltitude - m_position.altitude;
    if (std::abs(altDiff) > 50.0) {
      m_vspeed = std::clamp(altDiff * ALT_P_GAIN, -ALT_MAX, ALT_MAX);
    } else {
      m_vspeed = 0;
      m_position.altitude = m_targetAltitude;
    }
    m_position.altitude += m_vspeed * dt;
  }
}

void Aircraft::controlVspeed(double dt) {
  double diff = m_targetVspeed - m_vspeed;
  if (std::abs(diff) > 0.5)
    m_vspeed += std::copysign(2.0 * dt, diff);
  else
    m_vspeed = m_targetVspeed;
  m_position.altitude += m_vspeed * dt;
}

void Aircraft::approachLocalizer(double dt) {
}

void Aircraft::approachGlideslope(double dt) {
}

HoldingEntryType Aircraft::determineEntryType() const {
  if (!m_holdingPattern) return HoldingEntryType::UNDETERMINED;
  return determineHoldingEntry(
    m_heading,
    m_holdingPattern->inboundRadial,
    m_holdingPattern->standard
  );
}

void Aircraft::tickHoldingEntry(double dt) {
  if (!m_holdingPattern) return;

  controlSpeed(dt);
  HoldingPattern &hp = *m_holdingPattern;

  if (!m_holdingEntryDone) {
    m_holdingEntryType = determineEntryType();
    m_holdingEntryDone = true;
    m_holdingTimer = 0.0;
    hp.inboundTrack = hp.inboundRadial;
    hp.outboundHeading = normalizeHeading(hp.inboundRadial + (hp.standard ? 180.0 : -180.0));
    emitRequest({RequestType::HOLDING_ENTRY_STARTED, m_callsign, ""});
  }

  m_holdingEntryTimer += dt;

  switch (m_holdingEntryType) {
    case HoldingEntryType::DIRECT: {
      m_targetHeading = hp.outboundHeading;
      if (m_holdingTimer < 2.0) {
        m_heading = turnToward(m_heading, m_targetHeading, TURN_RATE_AIR, dt);
        m_holdingTimer += dt;
      } else {
        flyHeading(dt);
        m_holdingTimer += dt;
        if (m_holdingTimer > hp.legTime + 30.0) {
          m_state = AircraftState::HOLDING_PATTERN;
          m_holdingTimer = 0.0;
          m_holdingLegCount = 1;
          emitRequest({RequestType::HOLDING_ESTABLISHED, m_callsign, ""});
        }
      }
      break;
    }

    case HoldingEntryType::PARALLEL: {
      if (m_holdingTimer < hp.legTime) {
        m_targetHeading = hp.outboundHeading;
        flyHeading(dt);
        m_holdingTimer += dt;
      } else {
        m_targetHeading = hp.inboundRadial;
        flyHeading(dt);
        if (std::abs(headingError(m_heading, hp.inboundRadial)) < 10.0) {
          m_state = AircraftState::HOLDING_PATTERN;
          m_holdingTimer = 0.0;
          m_holdingLegCount = 1;
          emitRequest({RequestType::HOLDING_ESTABLISHED, m_callsign, ""});
        }
      }
      break;
    }

    case HoldingEntryType::TEARDROP: {
      if (m_holdingTimer < hp.legTime) {
        m_targetHeading = normalizeHeading(hp.outboundHeading +
                          (hp.standard ? 30.0 : -30.0));
        flyHeading(dt);
        m_holdingTimer += dt;
      } else {
        m_targetHeading = hp.inboundRadial;
        flyHeading(dt);
        if (std::abs(headingError(m_heading, m_targetHeading)) < 10.0) {
          m_state = AircraftState::HOLDING_PATTERN;
          m_holdingTimer = 0.0;
          m_holdingLegCount = 1;
          emitRequest({RequestType::HOLDING_ESTABLISHED, m_callsign, ""});
        }
      }
      break;
    }

    default:
      break;
  }
}

void Aircraft::tickHoldingPattern(double dt) {
  if (!m_holdingPattern) return;

  controlSpeed(dt);
  HoldingPattern &hp = *m_holdingPattern;
  m_holdingTimer += dt;

  double turnTime = 180.0 / HOLDING_TURN_RATE;

  if (!m_holdingTurning) {
    if (m_holdingTimer < hp.legTime) {
      m_targetHeading = m_heading;
      flyHeading(dt);
    } else {
      m_holdingTurning = true;
      m_holdingTimer = 0.0;
      ++m_holdingLegCount;
    }
  } else {
    if (m_holdingTimer < turnTime) {
      double turnDir = hp.standard ? 1.0 : -1.0;
      m_heading = normalizeHeading(m_heading + turnDir * HOLDING_TURN_RATE * dt);
      m_targetHeading = m_heading;
      flyHeading(dt);
    } else {
      m_holdingTurning = false;
      m_holdingTimer = 0.0;
    }
  }
}

void Aircraft::tickGround(double dt) {
  switch (m_state) {
    case AircraftState::PUSHBACK: {
      m_speed = 5.0;
      double pushHdg = normalizeHeading(m_heading + 180.0);
      m_heading = turnToward(m_heading, pushHdg, TURN_RATE_GROUND, dt);
      double step = m_speed * KNOTS_TO_MS * dt;
      double hdgRad = toRad(m_heading);
      double lat = m_position.coordinates.latitude;
      m_position.coordinates.latitude += step * std::cos(hdgRad) / EARTH_M;
      m_position.coordinates.longitude += step * std::sin(hdgRad)
        / (EARTH_M * std::cos(toRad(lat)));
      m_pushbackDist += step;
      if (m_pushbackDist > 50.0) {
        m_pushbackDist = 0;
        m_state = AircraftState::PUSHBACK_COMPLETE;
        emitRequest({RequestType::PUSHBACK_COMPLETE, m_callsign, ""});
      }
      break;
    }

    case AircraftState::ENGINE_START:
      m_state = AircraftState::PUSHBACK_COMPLETE;
      break;

    case AircraftState::TAXIING:
    case AircraftState::BACK_TAXI:
      controlSpeed(dt);
      followPath(dt);
      break;

    case AircraftState::HOLDING_SHORT:
      m_speed = 0;
      break;
    case AircraftState::LINE_UP:
      controlSpeed(dt);
      followPath(dt);
      break;

    default:
      break;
  }
}

void Aircraft::tickTakeoff(double dt) {
  if (m_state == AircraftState::TAKEOFF_ROLL) {
    m_speed += TO_ACCEL * dt;
    if (m_speed > TO_ROTATE) {
      m_state = AircraftState::CLIMBOUT;
      m_targetSpeed = m_aircraftType ? m_aircraftType->perf.cruiseSpeed : 250.0;
      emitRequest({RequestType::ROTATION, m_callsign, ""});
    }
  }
}

void Aircraft::tickClimbout(double dt) {
  m_targetVspeed = CLIMBOUT_VS;
  if (m_targetAltitude > 0) {
    controlAltitude(dt);
  } else {
    controlVspeed(dt);
  }
  followPath(dt);
  if (m_position.altitude > CLIMBOUT_ALT) {
    m_state = AircraftState::CLIMBING;
    emitRequest({RequestType::CLIMBOUT_ESTABLISHED, m_callsign, ""});
  }
}

void Aircraft::tickClimbing(double dt) {
  if (m_targetAltitude > 0) {
    controlAltitude(dt);
  } else {
    m_targetVspeed = CLIMB_VS;
    controlVspeed(dt);
  }
  controlSpeed(dt);
  followPath(dt);
  if (m_position.altitude >= CRUISE_ALT || m_targetAltitude <= m_position.altitude) {
    m_state = AircraftState::CRUISING;
  }
}

void Aircraft::tickCruising(double dt) {
  controlSpeed(dt);
  followPath(dt);
  if ((m_targetAltitude > 0 && m_targetAltitude < m_position.altitude) ||
      (m_waypointIndex >= m_path.waypoints.size() && !m_path.waypoints.empty()
       && m_targetAltitude == 0)) {
    m_state = AircraftState::DESCENDING;
  }
}

void Aircraft::tickDescent(double dt) {
  if (m_targetAltitude > 0) {
    controlAltitude(dt);
  } else {
    m_targetVspeed = DESCEND_VS;
    controlVspeed(dt);
  }
  controlSpeed(dt);
  followPath(dt);

  if (m_position.altitude <= APPROACH_ALT && m_targetAltitude == 0) {
    m_position.altitude = APPROACH_ALT;
    m_state = AircraftState::APPROACH;
    emitRequest({RequestType::ESTABLISHED_ON_COURSE, m_callsign, "APPROACH"});
  }
}

void Aircraft::tickArrival(double dt) {
  tickDescent(dt);
}

void Aircraft::tickApproach(double dt) {
  m_targetVspeed = FINAL_VS;
  controlSpeed(dt);
  controlVspeed(dt);
  followPath(dt);

  if (m_position.altitude <= FINAL_ALT) {
    if (m_assignedApproach == ApproachType::CIRCLING) {
      m_state = AircraftState::CIRCLING_APPROACH;
    } else if (m_state == AircraftState::TOUCH_AND_GO) {
      m_state = AircraftState::LANDING_FLARE;
    } else if (m_state == AircraftState::STOP_AND_GO) {
      m_state = AircraftState::LANDING_FLARE;
    } else {
      m_state = AircraftState::FINAL_APPROACH;
    }
  }
}

void Aircraft::tickFinalApproach(double dt) {
  m_targetVspeed = LANDING_VS;
  controlSpeed(dt);
  controlVspeed(dt);
  followPath(dt);

  if (m_position.altitude <= LANDING_ALT) {
    m_position.altitude = LANDING_ALT;
    m_state = AircraftState::LANDING_FLARE;
    emitRequest({RequestType::TOUCHDOWN, m_callsign, ""});
  }
}

void Aircraft::tickLanding(double dt) {
  switch (m_state) {
    case AircraftState::LANDING_FLARE:
      m_speed *= std::pow(FLARE_DECEL, dt);
      if (m_speed < FLARE_SPD)
        m_state = AircraftState::ROLLING;
      break;

    case AircraftState::ROLLING:
      m_speed *= std::pow(ROLL_DECEL, dt);
      if (m_speed < ROLL_SPD)
        m_state = AircraftState::VACATING;
      break;

    case AircraftState::VACATING:
      m_speed = VACATE_SPD;
      followPath(dt);
      break;

    case AircraftState::TOUCH_AND_GO: {
      m_speed *= std::pow(FLARE_DECEL, dt);
      if (m_speed < TO_ROTATE * 0.7) {
        m_speed = TO_ROTATE * 0.5;
        m_state = AircraftState::TAKEOFF_ROLL;
        emitRequest({RequestType::TOUCH_AND_GO_INITIATED, m_callsign, ""});
      }
      break;
    }

    case AircraftState::STOP_AND_GO: {
      m_speed *= std::pow(ROLL_DECEL, dt);
      if (m_speed < 3.0) {
        m_speed = 0;
        m_stopTimer += dt;
        if (m_stopTimer > 3.0) {
          m_stopTimer = 0;
          m_state = AircraftState::TAKEOFF_ROLL;
        }
      }
      break;
    }

    default:
      break;
  }
}

void Aircraft::tickGoAround(double dt) {
  m_targetVspeed = GAROUND_VS;
  m_speed = GAROUND_SPD;
  controlVspeed(dt);
  followPath(dt);

  if (m_position.altitude > CLIMBOUT_ALT * 0.5) {
    m_state = AircraftState::CLIMBING;
    emitRequest({RequestType::GO_AROUND_COMPLETE, m_callsign, ""});
  }
}

void Aircraft::tickEmergency(double dt) {
  if (m_state == AircraftState::EMERGENCY_DESCENT) {
    controlVspeed(dt);
    m_speed = std::max(m_speed, 300.0);
    m_heading = turnToward(m_heading, m_targetHeading, TURN_RATE_AIR, dt);
    flyHeading(dt);
  } else {
    controlSpeed(dt);
    controlAltitude(dt);
    followPath(dt);
  }
}

void Aircraft::tick(double dt) {
  processCommandQueue();

  switch (m_state) {
    case AircraftState::PARKED:
    case AircraftState::INACTIVE:
      break;

    case AircraftState::PUSHBACK:
    case AircraftState::PUSHBACK_COMPLETE:
    case AircraftState::ENGINE_START:
    case AircraftState::TAXIING:
    case AircraftState::HOLDING_SHORT:
    case AircraftState::LINE_UP:
    case AircraftState::BACK_TAXI:
      tickGround(dt);
      break;

    case AircraftState::TAKEOFF_ROLL:
      tickTakeoff(dt);
      break;

    case AircraftState::CLIMBOUT:
      tickClimbout(dt);
      break;

    case AircraftState::CLIMBING:
      tickClimbing(dt);
      break;

    case AircraftState::CRUISING:
      tickCruising(dt);
      break;

    case AircraftState::DESCENDING:
      tickDescent(dt);
      break;

    case AircraftState::HOLDING_ENTRY:
      tickHoldingEntry(dt);
      break;

    case AircraftState::HOLDING_PATTERN:
      tickHoldingPattern(dt);
      break;

    case AircraftState::ARRIVAL:
      tickArrival(dt);
      break;

    case AircraftState::INITIAL_APPROACH:
    case AircraftState::VECTORS_TO_FINAL:
    case AircraftState::APPROACH:
    case AircraftState::FINAL_APPROACH:
      tickApproach(dt);
      break;

    case AircraftState::CIRCLING_APPROACH:
      tickApproach(dt);
      break;

    case AircraftState::LANDING_FLARE:
    case AircraftState::ROLLING:
    case AircraftState::VACATING:
    case AircraftState::TOUCH_AND_GO:
    case AircraftState::STOP_AND_GO:
    case AircraftState::LOW_APPROACH:
      tickLanding(dt);
      break;

    case AircraftState::GO_AROUND:
    case AircraftState::MISSED_APPROACH:
      tickGoAround(dt);
      break;

    case AircraftState::EMERGENCY:
    case AircraftState::EMERGENCY_DESCENT:
    case AircraftState::RADIO_FAILURE:
    case AircraftState::MINIMUM_FUEL:
      tickEmergency(dt);
      break;
  }

}

} // namespace Sim
