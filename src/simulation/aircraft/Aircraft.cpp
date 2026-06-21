#include "Aircraft.h"
#include <spdlog/spdlog.h>

namespace Sim {
Aircraft::Aircraft() = default;
Aircraft::~Aircraft() = default;

void Aircraft::tick(double dt) {
  spdlog::info(std::format("Aircraft::tick({})", dt));
}
} // namespace Sim
