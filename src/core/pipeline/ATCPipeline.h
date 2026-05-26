#pragma once
#include <condition_variable>

#include "utils/MicrophoneActivationType.h"
#include <functional>
#include <mutex>
#include <thread>

#include "ATCEventType.h"
#include "llm/llamacpp/LlamacppBackend.h"
#include "stt/Whisper/WhisperBackend.h"
#include "tts/Voxtral/VoxtralBackend.h"
#include "utils/NoiseSuppressionType.h"

namespace Pipeline {
    struct ATCPipelineConfig {
        MicrophoneActivationType microphoneActivationType;
        NoiseSuppressionType noiseSuppressionType;

        std::function<void(ATCEventType, std::string_view)> onEventCallback;

        // TODO: remove and make proper initialzation

    };

    class ATCPipeline {
    public:
        explicit ATCPipeline(const ATCPipelineConfig& config);
        ~ATCPipeline();

        void start();
        void stop();

        void reloadConfig(const ATCPipelineConfig& config);

        void microphoneStartRecording();
        void microphoneStopRecording();

    private:
        void workerThread();

        ATCPipelineConfig m_config;
        std::jthread m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cv;

        LLM::LlamacppBackend m_llmBackend;
        STT::WhisperBackend m_sttBackend;
        TTS::VoxtralBackend m_ttsBackend;
    };
}
