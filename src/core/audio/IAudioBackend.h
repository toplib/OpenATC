#pragma once

#include "Sound.h"

namespace Audio {
class IAudioBackend {
public:
  virtual ~IAudioBackend() = default;

  virtual void pushAudioFrame(const Sound &sound) = 0;
  virtual void play() = 0;
};
} // namespace Audio
