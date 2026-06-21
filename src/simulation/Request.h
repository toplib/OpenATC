#pragma once

#include "simulation/RequestType.h"
#include <string>

namespace Sim {

struct Request {
  RequestType type;
  std::string callsign;
  std::string data;
};

} // namespace Sim
