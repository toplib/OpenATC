#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "navigation/Coordinates.h"
#include "navigation/HoldingPattern.h"
#include "navigation/WorldPosition.h"
#include "simulation/Command.h"
#include "simulation/CommandType.h"
#include "simulation/FlightPlan.h"
#include "simulation/Path.h"
#include "simulation/Request.h"
#include "simulation/Waypoint.h"
#include "simulation/aircraft/AircraftState.h"
#include "simulation/aircraft/AircraftType.h"

namespace Sim {

using RequestCallback = std::function<void(const Request &)>;

class Aircraft {
public:
  Aircraft();
  ~Aircraft();

  void tick(double dt);
  void executeCommand(const Command &cmd);

  void setState(AircraftState state);
  AircraftState getState() const;

  void setCallsign(const std::string &cs);
  const std::string &getCallsign() const;

  void setPosition(const WorldPosition &pos);
  const WorldPosition &getPosition() const;
  void setHeading(double heading);
  double getHeading() const;
  void setSpeed(double knots);
  double getSpeed() const;
  double getVSpeed() const;
  size_t getWaypointIndex() const;

  void setPath(const Path &path);
  void setFlightPlan(const FlightPlan &fp);
  const FlightPlan* getFlightPlan() const;

  void setAircraftType(const AircraftType &type);
  const AircraftType* getAircraftType() const;

  enum class ApproachType {
    NONE,
    VISUAL,
    ILS,
    LOCALIZER_ONLY,
    VOR,
    NDB,
    RNAV_GPS,
    CIRCLING,
  };
  void assignApproach(ApproachType type, const std::string &runway);
  ApproachType getAssignedApproach() const;
  std::string getAssignedRunway() const;

  void assignHoldingPattern(const HoldingPattern &pattern);
  bool isInHolding() const;
  std::optional<HoldingPattern> getHoldingPattern() const;

  void setSquawk(int code);
  int getSquawk() const;

  void declareEmergency(const std::string &nature);
  void cancelEmergency();
  bool isEmergency() const;
  std::string getEmergencyNature() const;

  void setFrequency(double freq);
  double getFrequency() const;
  void setController(const std::string &controller);
  std::string getController() const;

  void setTargetAltitude(double alt);
  double getTargetAltitude() const;

  std::optional<Command> getLastCommand() const;

  void setRequestCallback(RequestCallback cb);

private:
  void tickGround(double dt);
  void tickTakeoff(double dt);
  void tickClimbout(double dt);
  void tickClimbing(double dt);
  void tickCruising(double dt);
  void tickDescent(double dt);
  void tickArrival(double dt);
  void tickApproach(double dt);
  void tickFinalApproach(double dt);
  void tickLanding(double dt);
  void tickHolding(double dt);
  void tickGoAround(double dt);
  void tickEmergency(double dt);

  void flyHeading(double dt);
  void followPath(double dt);
  void controlSpeed(double dt);
  void controlAltitude(double dt);
  void controlVspeed(double dt);
  void approachLocalizer(double dt);
  void approachGlideslope(double dt);

  void setInitialHeadingToWaypoint();

  void tickHoldingEntry(double dt);
  void tickHoldingPattern(double dt);
  HoldingEntryType determineEntryType() const;

  void processCommandQueue();
  bool preconditionMet(const Command &cmd);
  void executeImmediate(const Command &cmd);
  void processGroundCommand(const Command &cmd);
  void processTakeoffCommand(const Command &cmd);
  void processHeadingCommand(const Command &cmd);
  void processAltitudeCommand(const Command &cmd);
  void processSpeedCommand(const Command &cmd);
  void processApproachCommand(const Command &cmd);
  void processHoldingCommand(const Command &cmd);
  void processEmergencyCommand(const Command &cmd);
  void processCommCommand(const Command &cmd);

  void emitRequest(const Request &r);

  WorldPosition m_position;
  std::string m_callsign;

  double m_speed;
  double m_targetSpeed;
  double m_vspeed;
  double m_targetVspeed;

  double m_heading;
  double m_targetHeading;
  AircraftState m_state;

  Path m_path;
  size_t m_waypointIndex;

  double m_targetAltitude;

  std::vector<Command> m_queuedCommands;

  std::optional<FlightPlan> m_flightPlan;
  const AircraftType *m_aircraftType = nullptr;

  ApproachType m_assignedApproach = ApproachType::NONE;
  std::string m_assignedRunway;

  std::optional<HoldingPattern> m_holdingPattern;
  HoldingEntryType m_holdingEntryType = HoldingEntryType::UNDETERMINED;
  double m_holdingTimer = 0.0;
  int m_holdingLegCount = 0;
  bool m_holdingTurning = false;
  double m_holdingEntryTimer = 0.0;
  bool m_holdingEntryDone = false;

  int m_squawk = 1200;

  bool m_emergency = false;
  std::string m_emergencyNature;
  bool m_squawk7700 = false;
  bool m_squawk7600 = false;

  double m_frequency = 0.0;
  std::string m_controller;

  bool m_radioFailure = false;
  double m_pushbackDist = 0.0;
  double m_stopTimer = 0.0;

  std::optional<Command> m_lastCommand;
  RequestCallback m_requestCb;
};

} // namespace Sim
