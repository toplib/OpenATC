#pragma once

#include <future>
#include <string>
#include <vector>

namespace TTS {

struct AudioOutput {
    std::vector<float> samples;
    int sampleRate;
};

class ITTSBackend {
public:
    virtual ~ITTSBackend() = default;
    virtual std::future<AudioOutput> speak(const std::string& text) = 0;
};

}
