#pragma once

#include "simulation/CommandType.h"
#include <string>

namespace Sim {

struct Command {
  CommandType type;
  std::string callsign;
  std::string arguments;
  std::string data;  // extra data (e.g., controller name, fix name, runway)
};

} // namespace Sim
