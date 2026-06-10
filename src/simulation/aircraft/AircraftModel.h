#pragma once

#include <string>

struct AircraftName {
  std::string icao;
  std::string name;
};

// ICAO code of airplane
enum class AircraftModel { B738, A320 };

static AircraftName getName(AircraftModel model) { return {"", ""}; }
