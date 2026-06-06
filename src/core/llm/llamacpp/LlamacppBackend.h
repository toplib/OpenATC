#pragma once
#include <filesystem>
#include <unordered_set>

#include "llm/ILLMBackend.h"
#include "llama.h"

namespace LLM {
    struct LlamacppConfig {
        std::filesystem::path model;
        uint32_t contextSize;
        uint32_t batchSize;
        uint32_t threads;
        bool generateUntilEOS = false;
        int tokenLimit = 200;
    };
    class LlamacppBackend : public ILLMBackend {
    public:
        explicit LlamacppBackend(const LlamacppConfig& config);
        ~LlamacppBackend() override;

        void setHistory(const std::vector<Message> &history) override;
        const std::vector<Message>& getHistory() const override;

        Message getResponse(const Message& message) override;
    private:
        LlamacppConfig m_config;
        std::vector<Message> m_history;

        llama_model* m_model = nullptr;
        llama_context* m_ctx = nullptr;
        llama_sampler* m_sampler = nullptr;
        llama_memory_t m_memory = nullptr;
        std::unordered_set<llama_token> m_eog_tokens;

        void clearKVCache();
        [[nodiscard]] std::string buildPrompt() const;

    };
}
