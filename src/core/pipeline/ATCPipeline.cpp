#include "ATCPipeline.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>

namespace Pipeline {
ATCPipeline::ATCPipeline(ATCPipelineConfig config)
    : m_config(std::move(config)) {}

ATCPipeline::~ATCPipeline() = default;

void ATCPipeline::workerThread(std::stop_token token) {
  while (!token.stop_requested()) {
    if (true) {
    }
  }
}

void ATCPipeline::start() {
  m_thread =
      std::jthread([this](std::stop_token token) { workerThread(token); });

  if (!m_config.microphone) {
    spdlog::error("ATCPipeline::start() failed to start microphone. "
                  "m_conifg.microphone is nullptr");
    throw std::runtime_error("ATCPipeline::start() failed to start microphone. "
                             "m_conifg.microphone is nullptr");
  }
  m_audioData = m_config.microphone->start();
}

void ATCPipeline::stop() { m_thread.request_stop(); }
} // namespace Pipeline
