#pragma once
#include "core/microphone/MicrophoneInputAudioData.h"

namespace Pipeline {
class MicrophonePipeline {
public:
  MicrophonePipeline();
  ~MicrophonePipeline();

  MicrophoneInputAudioData *start();
  void stop();
};
} // namespace Pipeline
