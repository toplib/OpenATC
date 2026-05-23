#include "Microphone.h"

#include <stdexcept>
#include <sstream>
#include <math.h>

namespace Input {


    Microphone::Microphone(MicrophoneConfig& config) : m_config(config) {
        auto error = Pa_Initialize();
        if (error != paNoError) {
            std::ostringstream oss;
            oss << "PortAudio error: " << Pa_GetErrorText(error);
            throw std::runtime_error(oss.str());
        }
    }


    Microphone::~Microphone() {
        stop();
    }

    AudioData* Microphone::start() {
        auto err = Pa_OpenDefaultStream(
            &m_stream,
            m_config.channels,
            0, // Output channels
            paFloat32,
            m_config.sampleRate,
            512,
            microphoneCallback,
            &m_data
        );
        if (err != paNoError) {
            std::ostringstream oss;
            oss << "PortAudio error: " << Pa_GetErrorText(err);
            throw std::runtime_error(oss.str());
        }

        Pa_StartStream(m_stream);
        return &m_data;
    }

    void Microphone::stop() {
        Pa_StopStream(m_stream);
        Pa_CloseStream(m_stream);
        Pa_Terminate();
    }

    int Microphone::microphoneCallback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData) {
        (void) outputBuffer;
        (void) timeInfo;
        (void) statusFlags;

        auto* data = static_cast<AudioData*>(userData);

        const float* in = static_cast<const float*>(inputBuffer);

        if (!in) {
            return paContinue;
        }

        std::lock_guard<std::mutex> lock(data->mutex);

        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            data->samples.push(in[i]);
        }
        return paContinue;
    }
}
