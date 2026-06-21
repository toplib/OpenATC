#pragma once

#include <string>

namespace Sim {

enum class AircraftCategory {
  LIGHT,
  SMALL,
  MEDIUM,
  LARGE,
  HEAVY,
  SUPER,
  MILITARY,
};

struct AircraftPerformance {
  double maxSpeed;
  double cruiseSpeed;
  double maxAltitude;
  double climbRate;
  double descentRate;
  double takeoffSpeed;
  double approachSpeed;
  double maxHoldingSpeed;
  double turnRate;
  double maxRange;
  bool rnavCapable;
  bool ilsCapable;
  bool rnpCapable;
};

struct AircraftType {
  std::string icao;
  std::string manufacturer;
  std::string model;
  AircraftCategory category;
  double wakeTurbulenceCategory;
  AircraftPerformance perf;
};

namespace AircraftDatabase {

inline const AircraftType B738{
  "B738", "Boeing", "737-800", AircraftCategory::MEDIUM, 2.0,
  {530, 453, 41000, 2200, 1800, 155, 145, 280, 3.0, 3200, true, true, true}
};

inline const AircraftType A320{
  "A320", "Airbus", "A320-200", AircraftCategory::MEDIUM, 2.0,
  {518, 447, 39000, 2100, 1700, 148, 138, 280, 3.0, 3100, true, true, true}
};

inline const AircraftType B77W{
  "B77W", "Boeing", "777-300ER", AircraftCategory::HEAVY, 3.0,
  {560, 490, 43100, 1800, 1500, 165, 150, 265, 3.0, 7885, true, true, true}
};

inline const AircraftType A388{
  "A388", "Airbus", "A380-800", AircraftCategory::SUPER, 4.0,
  {560, 488, 43100, 1500, 1400, 175, 155, 250, 3.0, 8200, true, true, true}
};

inline const AircraftType C172{
  "C172", "Cessna", "172 Skyhawk", AircraftCategory::LIGHT, 1.0,
  {163, 126, 14000, 700, 500, 55, 55, 110, 3.0, 640, false, true, false}
};

inline const AircraftType CRJ9{
  "CRJ9", "Bombardier", "CRJ-900", AircraftCategory::SMALL, 2.0,
  {469, 410, 37000, 2200, 1700, 142, 133, 260, 3.0, 1550, true, true, true}
};

inline const AircraftType* getAircraftType(const std::string &icao) {
  if (icao == "B738") return &B738;
  if (icao == "A320") return &A320;
  if (icao == "B77W") return &B77W;
  if (icao == "A388") return &A388;
  if (icao == "C172") return &C172;
  if (icao == "CRJ9") return &CRJ9;
  return nullptr;
}

} // namespace AircraftDatabase
} // namespace Sim
