#include <spdlog/spdlog.h>

#include "tts/Voxtral/VoxtralBackend.h"

int main() {
    spdlog::info("TTS test");

    TTS::VoxtralConfig config{
        .model = "/home/toplib/CLionProjects/SimpleATC/models/voxtral-q4.gguf",
        .tokenizer = "/home/toplib/CLionProjects/SimpleATC/models/tekken.json",
        .voice = "/home/toplib/CLionProjects/SimpleATC/models/neutral_female.safetensors",
    };


    try {
        TTS::VoxtralBackend tts(config);
        spdlog::info("TTS model loaded");

        auto future = tts.speak("Hello world, this is a test of the voxtral text to speech system");
        TTS::AudioOutput output = future.get();

        spdlog::info("Generated {} audio samples at {} Hz ({} seconds)",
                     output.samples.size(),
                     output.sampleRate,
                     static_cast<double>(output.samples.size()) / output.sampleRate);
    } catch (const std::exception& e) {
        spdlog::error("TTS error: {}", e.what());
        return 1;
    }

    spdlog::info("TTS test completed");
    return 0;
}
