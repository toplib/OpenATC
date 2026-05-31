#pragma once

#include <mutex>
#include <queue>

struct MicrophoneInputAudioData {
  std::mutex mutex;
  std::queue<float> samples;
};
