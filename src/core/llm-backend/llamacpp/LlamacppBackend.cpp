#include "LlamacppBackend.h"

#include <iostream>

namespace LLM {

LlamacppBackend::LlamacppBackend(LlamacppConfig config) {
    m_config = config;

    llama_backend_init();

    llama_model_params mparams = llama_model_default_params();
    m_model = llama_model_load_from_file(
        m_config.model.c_str(),
        mparams
    );

    if (!m_model) {
        throw std::runtime_error("Cannot load model");
    }

    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx     = m_config.contextSize;
    cparams.n_batch   = m_config.batchSize;
    cparams.n_threads = m_config.threads;

    m_ctx = llama_init_from_model(m_model, cparams);

    if (!m_ctx) {
        llama_model_free(m_model);
        throw std::runtime_error("Failed to create context");
    }

    m_sampler = llama_sampler_chain_init(llama_sampler_chain_default_params());

    llama_sampler_chain_add(m_sampler, llama_sampler_init_greedy());
    llama_sampler_chain_add(m_sampler, llama_sampler_init_top_k(40));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_top_p(0.95f, 1));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(m_sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
}

LlamacppBackend::~LlamacppBackend() {
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

    auto promise = std::make_shared<std::promise<Message>>();
    std::future<Message> future = promise->get_future();

    try {
        Message result = {"assistant", ""};

        std::cerr << "[DEBUG] llama_model_get_vocab" << std::endl;
        const llama_vocab* vocab = llama_model_get_vocab(m_model);

        std::cerr << "[DEBUG] buildPrompt" << std::endl;
        std::string prompt = buildPrompt();

        std::vector<llama_token> tokens(prompt.size() + 8);
        std::cerr << "[DEBUG] llama_tokenize" << std::endl;

        int n_tokens = llama_tokenize(
            vocab,
            prompt.c_str(),
            static_cast<int32_t>(prompt.size()),
            tokens.data(),
            static_cast<int32_t>(tokens.size()),
            true,
            false
        );

        if (n_tokens < 0) {
            throw std::runtime_error("Tokenization failed");
        }

        std::cerr << "[DEBUG] tokens.resize" << std::endl;
        tokens.resize(n_tokens);

        std::cerr << "[DEBUG] llama_sampler_reset" << std::endl;
        llama_sampler_reset(m_sampler);

        std::cerr << "[DEBUG] llama_batch_init" << std::endl;
        llama_batch batch = llama_batch_init(
            static_cast<int32_t>(m_config.batchSize),
            0,
            1
        );

        int n_processed = 0;

        while (n_processed < n_tokens) {
            int n_chunk = std::min(
                static_cast<int>(m_config.batchSize),
                n_tokens - n_processed
            );

            for (int i = 0; i < n_chunk; i++) {
                batch.token[i]      = tokens[n_processed + i];
                batch.pos[i]        = n_processed + i;
                batch.seq_id[i][0] = 0;
                batch.n_seq_id[i]  = 1;

                batch.logits[i] = (n_processed + i == n_tokens - 1);
            }

            batch.n_tokens = n_chunk;

            std::cerr << "[DEBUG] llama_decode" << std::endl;
            std::cerr << "[DEBUG] n_tokens = " << n_tokens << std::endl;
            std::cerr << "[DEBUG] batchSize = " << m_config.batchSize << std::endl;
            if (llama_decode(m_ctx, batch) != 0) {
                llama_batch_free(batch);
                throw std::runtime_error(
                    "Failed to decode prompt"
                );
            }

            n_processed += n_chunk;
        }

        int cur_pos = n_tokens;
        const int limit = m_config.tokenLimit;

        for (int i = 0; m_config.generateUntilEOS || (i < limit); i++) {
            std::cerr << "[DEBUG] generation step " << i << std::endl;
            std::cerr << "[DEBUG] llama_sampler_sample" << std::endl;
            llama_token token = llama_sampler_sample(
                m_sampler,
                m_ctx,
                -1
            );
            std::cerr << "[DEBUG] token id = " << token << std::endl;
            std::cerr << "[DEBUG] llama_vocab_is_eog" << std::endl;
            if (llama_vocab_is_eog(vocab, token)) {
                std::cerr << "[DEBUG] EOG hit" << std::endl;
                break;
            }

            char buffer[256];

            std::cerr << "[DEBUG] llama_token_to_piece" << std::endl;
            int n = llama_token_to_piece(
                vocab,
                token,
                buffer,
                sizeof(buffer),
                0,
                true
            );

            if (n < 0) {
                break;
            }

            result.content.append(buffer, n);

            batch.n_tokens   = 1;
            batch.token[0]   = token;
            batch.pos[0]     = cur_pos++;
            batch.n_seq_id[0] = 1;
            batch.seq_id[0][0] = 0;
            batch.logits[0]  = true;

            std::cerr << "[DEBUG] about to decode generation token" << std::endl;
            std::cerr << "[DEBUG] llama_decode" << std::endl;
            if (llama_decode(m_ctx, batch) != 0) {
                std::cerr << "[DEBUG] generation decode failed" << std::endl;
                llama_batch_free(batch);
                break;
            }
        }

        std::cerr << "[DEBUG] llama_batch_free" << std::endl;
        llama_batch_free(batch);
        m_history.push_back(result);
        promise->set_value(result);

    } catch (...) {
        promise->set_exception(std::current_exception());
    }

    return future;
}

std::string LlamacppBackend::buildPrompt() const {
    std::vector<llama_chat_message> messages;
    std::vector<std::string> roles;
    std::vector<std::string> contents;

    roles.reserve(m_history.size());
    contents.reserve(m_history.size());

    for (const auto& [role, content] : m_history) {
        roles.push_back(role);
        contents.push_back(content);

        messages.push_back({
            roles.back().c_str(),
            contents.back().c_str()
        });
    }

    char tmpl_buf[100000];

    int tmpl_len = llama_model_meta_val_str(
        m_model,
        "tokenizer.chat_template",
        tmpl_buf,
        sizeof(tmpl_buf)
    );

    const char* tmpl = (tmpl_len > 0) ? tmpl_buf : nullptr;

    int n = llama_chat_apply_template(
        tmpl,
        messages.data(),
        messages.size(),
        true,
        nullptr,
        0
    );

    if (n < 0) {
        throw std::runtime_error(
            "llama_chat_apply_template failed"
        );
    }

    std::vector<char> buf(n + 1);

    llama_chat_apply_template(
        tmpl,
        messages.data(),
        messages.size(),
        true,
        buf.data(),
        n + 1
    );

    return std::string(buf.data(), n);
}

}