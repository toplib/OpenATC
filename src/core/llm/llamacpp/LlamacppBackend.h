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
        LlamacppBackend(LlamacppConfig config);
        ~LlamacppBackend() override;

        void setHistory(const std::vector<Message> &history) override;
        std::vector<Message>& getHistory() override;

        std::future<Message> getResponse(const Message& message) override;
    private:
        LlamacppConfig m_config;
        std::vector<Message> m_history;
        std::thread m_thread;

        llama_model* m_model = nullptr;
        llama_context* m_ctx = nullptr;
        llama_sampler* m_sampler = nullptr;
        int m_cur_pos = 0;
        std::string m_prev_prompt;
        std::unordered_set<llama_token> m_eog_tokens;

        [[nodiscard]] std::string buildPrompt() const;

    };
}
