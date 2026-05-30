#pragma once
#include <mutex>
#include <portaudio.h>
#include <queue>

#include "AudioData.h"

namespace Input {
    struct MicrophoneConfig {
        int channels = 1;
        int sampleRate = 16000;
        int numSeconds = 4;
    };


    class Microphone {
    public:
        Microphone(MicrophoneConfig& config);
        ~Microphone();

        AudioData* start();
        void stop();
    private:
        AudioData m_data = {};
        PaStream* m_stream = nullptr;
        MicrophoneConfig& m_config;

        static int microphoneCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);
    };
}
