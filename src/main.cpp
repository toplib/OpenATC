#include "gui/Manager.h"
#include "sherpa-onnx/c-api/cxx-api.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>

static void checkAlError(const char* label) {
    ALenum err = alGetError();
    if (err != AL_NO_ERROR)
        std::cerr << "OpenAL error after " << label << ": 0x" << std::hex << err << std::dec << std::endl;
}

inline bool playSound(const std::vector<float>& samples, int sampleRate) {
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return false;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(device);
        return false;
    }
    alcMakeContextCurrent(context);
    checkAlError("makeContextCurrent");

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerf(AL_GAIN, 1.0f);

    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    checkAlError("genBuffers");
    alGenSources(1, &source);
    checkAlError("genSources");

    std::vector<int16_t> pcmData(samples.size());
    for (size_t i = 0; i < samples.size(); ++i) {
        float sample = samples[i];
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        pcmData[i] = static_cast<int16_t>(sample * 32767.0f);
    }

    alBufferData(buffer, AL_FORMAT_MONO16, pcmData.data(),
                 pcmData.size() * sizeof(int16_t), sampleRate);
    checkAlError("bufferData");

    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    alSourcei(source, AL_BUFFER, buffer);
    checkAlError("sourcei buffer");

    alSourcePlay(source);
    checkAlError("sourcePlay");

    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        std::cerr << "Source failed to start playing" << std::endl;
        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return false;
    }

    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return true;
}

int main(int argc, char* argv[]) {
    {
        using namespace sherpa_onnx::cxx;
        OfflineTtsConfig config;
        config.model.kokoro.model = "../models/kokoro-en-v0_19/model.onnx";
        config.model.kokoro.voices = "../models/kokoro-en-v0_19/voices.bin";
        config.model.kokoro.tokens = "../models/kokoro-en-v0_19/tokens.txt";
        config.model.kokoro.data_dir = "../models/kokoro-en-v0_19/espeak-ng-data";
        config.model.num_threads = 2;
        config.model.debug = 0;

        auto tts = OfflineTts::Create(config);
        if (tts.Get()) {
            spdlog::info("tts.Generate()");
            auto audio = tts.Generate(
                "Belavia seven five eight one, cleared for takeoff runway two seven right, after departure, contact departure one three five decimal two zero zero");
            if (!audio.samples.empty()) {
                std::cout << "Generated " << audio.samples.size()
                          << " samples at " << audio.sample_rate
                          << " Hz" << std::endl;
                spdlog::info(".");
                playSound(audio.samples, audio.sample_rate);
            }
        }
    }

    GUI::Manager manager(argc, argv);
    return manager.run();
}