#include <spdlog/spdlog.h>

#include "microphone/Microphone.h"
#include "stt/Whisper/WhisperBackend.h"

int main(int argc, char *argv[]) {
    spdlog::info("STT test");

    // Microphone initializing
    Input::MicrophoneConfig microphone_config;
    Input::Microphone microphone(microphone_config);

    // Whisper backend initializing
    STT::WhisperConfig whisper_config = {.model = "/home/toplib/CLionProjects/SimpleATC/models/ggml-large-v3.bin"};
    STT::WhisperBackend backend(whisper_config);

    std::vector<std::int16_t> buffer;
    buffer.reserve(16000 * 3);

    Input::AudioData* data = microphone.start();
    while (true) {
        {
            std::lock_guard<std::mutex> lock(data->mutex);

            while (!data->samples.empty()) {
                float sample = data->samples.front();
                data->samples.pop();

                sample = std::clamp(sample, -1.0f, 1.0f);

                std::int16_t s16 =
                    static_cast<std::int16_t>(sample * 32767.0f);

                buffer.push_back(s16);
            }
        }

        if (buffer.size() >= 16000 * 2) { // ~2 seconds
            std::vector<std::int16_t> chunk;
            chunk.swap(buffer);

            std::span<const std::int16_t> span(chunk);
            auto future = backend.transcribe(span);

            std::string text = future.get();
            spdlog::info("Transcription: {}", text);
        }
    }

    return 0;
}
