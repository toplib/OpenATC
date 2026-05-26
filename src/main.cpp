#include <spdlog/spdlog.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "tts/Voxtral/VoxtralBackend.h"

static void play_audio(const std::vector<float>& samples, int sampleRate) {
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        spdlog::error("Failed to open OpenAL device");
        return;
    }
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context) {
        alcCloseDevice(device);
        spdlog::error("Failed to create OpenAL context");
        return;
    }
    alcMakeContextCurrent(context);

    ALuint buffer, source;
    alGenBuffers(1, &buffer);

    std::vector<short> pcm(samples.size());
    for (size_t i = 0; i < samples.size(); ++i) {
        float s = std::clamp(samples[i], -1.0f, 1.0f);
        pcm[i] = static_cast<short>(s * 32767.0f);
    }

    alBufferData(buffer, AL_FORMAT_MONO16, pcm.data(),
                 static_cast<ALsizei>(pcm.size() * sizeof(short)), sampleRate);

    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    ALint state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

int main() {
    spdlog::info("TTS test");

    TTS::VoxtralConfig config{
        .model = "/home/toplib/CLionProjects/SimpleATC/models/voxtral-tts-q4.gguf",
        .tokenizer = "/home/toplib/CLionProjects/SimpleATC/models/tekken.json",
        .voice = "/home/toplib/CLionProjects/SimpleATC/models/neutral_female.safetensors",
    };


    try {
        TTS::VoxtralBackend tts(config);
        spdlog::info("TTS model loaded");

        auto future = tts.speak("Belavia seven five eight one cleared for takeoff, runway two seven romeo, wind zero eight five at one five knots, after departure contact departure, one three five decimal two zero zero");
        TTS::AudioOutput output = future.get();

        spdlog::info("Generated {} audio samples at {} Hz ({} seconds)",
                     output.samples.size(),
                     output.sampleRate,
                     static_cast<double>(output.samples.size()) / output.sampleRate);

        play_audio(output.samples, output.sampleRate);
    } catch (const std::exception& e) {
        spdlog::error("TTS error: {}", e.what());
        return 1;
    }

    spdlog::info("TTS test completed");
    return 0;
}
