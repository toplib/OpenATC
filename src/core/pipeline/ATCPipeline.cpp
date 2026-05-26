#include "ATCPipeline.h"

namespace Pipeline {
    ATCPipeline::ATCPipeline(const ATCPipelineConfig &config) : m_config(config),

    m_llmBackend({
        .model = "/run/media/toplib/Новый том/llms/lmstudio-community/gemma-4-E2B-it-GGUF/gemma-4-E2B-it-Q4_K_M.gguf",
        .contextSize = 2048,
        .batchSize = 512,
        .threads = 8,
        .generateUntilEOS = true,
        .tokenLimit = 200
    }), m_sttBackend({
        .model = "/home/toplib/CLionProjects/SimpleATC/models/ggml-large-v3-turbo-q8_0.bin"
    }), m_ttsBackend({
        .model = "/home/toplib/CLionProjects/SimpleATC/models/voxtral-tts-q4.gguf",
        .tokenizer = "/home/toplib/CLionProjects/SimpleATC/models/tekken.json",
        .voice = "neutral_female.safetensors"
    }) {

    }
    ATCPipeline::~ATCPipeline() = default;

    void ATCPipeline::reloadConfig(const ATCPipelineConfig &config) {
        m_config = config;
    }
}
