#pragma once

#include "utils/MicrophoneActivationType.h"
#include <functional>
#include <mutex>
#include <thread>

#include "ATCEventType.h"
#include "llm/ILLMBackend.h"
#include "stt/ISTTBackend.h"
#include "tts/ITTSBackend.h"
#include "utils/NoiseSuppressionType.h"

namespace Pipeline {
    struct ATCPipelineConfig {
        MicrophoneActivationType microphoneActivationType;
        NoiseSuppressionType noiseSuppressionType; // TODO: Implement noise suppression

        std::function<void(ATCEventType, std::string_view)> onEventCallback;

        std::unique_ptr<LLM::ILLMBackend> llmBackend;
        std::unique_ptr<STT::ISTTBackend> sttBackend;
        std::unique_ptr<TTS::ITTSBackend> ttsBackend;
    };

    class ATCPipeline {
    public:
        ATCPipeline(const ATCPipelineConfig& config);
        ~ATCPipeline();

        void start();
        void stop();
    private:
        void workerThread(std::stop_token token);

        const ATCPipelineConfig* m_config;
        std::jthread m_thread;
        std::mutex m_mutex;
    };
}
