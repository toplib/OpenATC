#include "LlamacppBackend.h"

namespace LLM {
    LlamacppBackend::LlamacppBackend(LlamacppConfig config) {
        m_config = config;
        llama_backend_init();
        llama_model_params mparams = llama_model_default_params();
        m_model = llama_model_load_from_file(m_config.model.c_str(), mparams);

        if (!m_model) {
            throw std::runtime_error("Cannot load model");
        }

        llama_context_params cparams = llama_context_default_params();
        cparams.n_ctx = m_config.contextSize;
        cparams.n_batch = m_config.batchSize;
        cparams.n_threads = m_config.threads;

        m_ctx = llama_init_from_model(m_model, cparams);
        if (!m_ctx) {
            llama_model_free(m_model);
            throw std::runtime_error("Failed to create context");
        }
        m_sampler = llama_sampler_chain_init(llama_sampler_chain_default_params());
        llama_sampler_chain_add(m_sampler, llama_sampler_init_greedy());
    }

    LlamacppBackend::~LlamacppBackend() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
        llama_sampler_free(m_sampler);
        llama_free(m_ctx);
        llama_model_free(m_model);
        llama_backend_free();
    }

    void LlamacppBackend::setHistory(const std::vector<Message>& history) {
        m_history = history;
    }

    std::vector<Message>& LlamacppBackend::getHistory() {
        return m_history;
    }

    std::future<Message> LlamacppBackend::getResponse(const Message& message) {
        m_history.push_back(message);
        Message result = {"assistant", ""};

        if (m_thread.joinable()) {
            m_thread.join();
        }

        auto promise = std::make_shared<std::promise<Message>>();
        std::future<Message> future = promise->get_future();

        // m_thread = std::thread([this, promise]() {
        //     try {
        //         Message result = {"assistant", ""};
        //
        //         const llama_vocab* vocab = llama_model_get_vocab(m_model);
        //
        //     }
        // });



        m_history.push_back(result);

        std::promise<Message> p;
        p.set_value(result);
        return p.get_future();
    }

    std::string LlamacppBackend::buildPrompt() const {
        std::string buffer;

        buffer.reserve(4096);

        buffer += "<|begin_of_text|>\n";

        for (const auto&[role, content] : m_history) {
            if (role == "system") {
                buffer += "<|system|>\n" + content + "\n<|end|>\n";
            } else if (role == "user") {
                buffer += "<|user|>\n" + content + "\n<|end|>\n";
            } else if (role == "assistant") {
                buffer += "<|assistant|>\n" + content + "\n<|end|>\n";
            }
        }
        buffer += "<|assistant|>\n";
        return buffer;
    }
}
