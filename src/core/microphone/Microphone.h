#pragma once
#include <mutex>
#include <portaudio.h>
#include <queue>

namespace Input {
    struct MicrophoneConfig {
        int channels = 1;
        int sampleRate = 44100;
        int numSeconds = 4;
    };
    struct AudioData {
        std::mutex mutex;
        std::queue<float> samples;
    };

    class Microphone {
    public:
        Microphone(MicrophoneConfig& config);
        ~Microphone();

        AudioData* start();
        void stop();

        static int microphoneCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);
    private:
        AudioData m_data = {};
        PaStream* m_stream = nullptr;
        MicrophoneConfig& m_config;
    };
}
