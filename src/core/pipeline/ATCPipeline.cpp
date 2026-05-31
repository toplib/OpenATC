#include "ATCPipeline.h"

namespace Pipeline {
ATCPipeline::ATCPipeline(const ATCPipelineConfig &config)
    : m_config(&config),
      m_thread([this](std::stop_token token) { workerThread(token); }) {}
ATCPipeline::~ATCPipeline() = default;

void ATCPipeline::workerThread(std::stop_token token) {
  while (!token.stop_requested()) {
  }
}

void ATCPipeline::start() {}

void ATCPipeline::stop() { m_thread.request_stop(); }
} // namespace Pipeline
