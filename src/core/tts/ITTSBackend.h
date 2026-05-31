#pragma once

#include "audio/Sound.h"
#include <string>

namespace TTS {

class ITTSBackend {
public:
  virtual ~ITTSBackend() = default;
  virtual Sound speak(const std::string &text) = 0;
};

} // namespace TTS
