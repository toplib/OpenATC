#include "MicrophonePipeline.h"
#include <spdlog/spdlog.h>

namespace Pipeline {
MicrophonePipeline::MicrophonePipeline() = default;
MicrophonePipeline::~MicrophonePipeline() = default;

MicrophoneInputAudioData *MicrophonePipeline::start() {
  spdlog::info("Starting capture data from microphone");
  return nullptr; // TODO: Make real microphone initialization with VAD or Push
                  // to toggle;
}

void MicrophonePipeline::stop() {
  spdlog::info("Stopped capturing data from microphone");
}

} // namespace Pipeline
