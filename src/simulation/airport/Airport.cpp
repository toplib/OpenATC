#include "Airport.h"

namespace Simulation {
Airport::Airport(AirportConfig config) : m_config(config) {
  // prefill with default parameters
  // TODO: Fetch all data from AIRAC and OpenATC own binary format.
  // Fetch all airports frequencies, and nearby points
  m_icao = "UMMS";
  m_iata = "MSQ";
}
Airport::~Airport() = default;
} // namespace Simulation
