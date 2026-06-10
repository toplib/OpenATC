#include "LlamacppBackend.h"

#include <iostream>

#include "chat.h"

namespace LLM {

LlamacppBackend::LlamacppBackend(const LlamacppConfig &config) {
  m_config = config;

  llama_backend_init();

  llama_model_params mparams = llama_model_default_params();
  m_model = llama_model_load_from_file(m_config.model.c_str(), mparams);

  if (!m_model) {
    throw std::runtime_error("Cannot load model");
  }

  const llama_vocab *vocab = llama_model_get_vocab(m_model);
  int vocab_size = llama_vocab_n_tokens(vocab);

  for (llama_token id = 0; id < vocab_size; id++) {
    if (llama_vocab_is_eog(vocab, id)) {
      m_eog_tokens.insert(id);
    }
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

  m_memory = llama_get_memory(m_ctx);

  m_sampler = llama_sampler_chain_init(llama_sampler_chain_default_params());
  llama_sampler_chain_add(m_sampler, llama_sampler_init_top_k(40));
  llama_sampler_chain_add(m_sampler, llama_sampler_init_top_p(0.95f, 1));
  llama_sampler_chain_add(m_sampler, llama_sampler_init_temp(0.8f));
  llama_sampler_chain_add(m_sampler,
                          llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
}

LlamacppBackend::~LlamacppBackend() {
  llama_sampler_free(m_sampler);
  llama_free(m_ctx);
  llama_model_free(m_model);
  llama_backend_free();
}

void LlamacppBackend::clearKVCache() { llama_memory_clear(m_memory, true); }

void LlamacppBackend::setHistory(const std::vector<Message> &history) {
  m_history = history;
  clearKVCache();
}

const std::vector<Message> &LlamacppBackend::getHistory() const {
  return m_history;
}

Message LlamacppBackend::getResponse(const Message &message) {
  m_history.push_back(message);

  Message result = {"assistant", ""};
  const llama_vocab *vocab = llama_model_get_vocab(m_model);
  const std::string prompt = buildPrompt();

  std::vector<llama_token> all_tokens(prompt.size() + 8);
  int n_all = llama_tokenize(
      vocab, prompt.c_str(), static_cast<uint32_t>(prompt.size()),
      all_tokens.data(), static_cast<uint32_t>(all_tokens.size()), true, false);

  if (n_all < 0) {
    throw std::runtime_error("Tokenization failed");
  }
  all_tokens.resize(n_all);

  clearKVCache();
  llama_sampler_reset(m_sampler);

  llama_batch batch =
      llama_batch_init(static_cast<uint32_t>(m_config.batchSize), 0, 1);

  int n_processed = 0;
  while (n_processed < n_all) {
    int n_chunk =
        std::min(static_cast<int>(m_config.batchSize), n_all - n_processed);

    batch.n_tokens = n_chunk;

    for (int i = 0; i < n_chunk; i++) {
      batch.token[i] = all_tokens[n_processed + i];
      batch.pos[i] = n_processed + i;
      batch.n_seq_id[i] = 1;
      batch.seq_id[i][0] = 0;
      batch.logits[i] = false;
    }

    batch.logits[n_chunk - 1] = true;

    if (llama_decode(m_ctx, batch) != 0) {
      llama_batch_free(batch);
      throw std::runtime_error("Prefill decode failed");
    }

    n_processed += n_chunk;
  }

  int cur_pos = n_all;
  const int limit = m_config.tokenLimit;

  for (int i = 0; m_config.generateUntilEOS || (i < limit); i++) {
    llama_token token = llama_sampler_sample(m_sampler, m_ctx, -1);

    if (m_eog_tokens.contains(token)) {
      break;
    }

    char buffer[256];
    int n = llama_token_to_piece(vocab, token, buffer, sizeof(buffer), 0, true);

    if (n < 0)
      break;

    result.content.append(buffer, n);

    batch.n_tokens = 1;
    batch.token[0] = token;
    batch.pos[0] = cur_pos++;
    batch.n_seq_id[0] = 1;
    batch.seq_id[0][0] = 0;
    batch.logits[0] = true;

    if (llama_decode(m_ctx, batch) != 0) {
      llama_batch_free(batch);
      throw std::runtime_error("Decode failed during generation");
    }
  }

  llama_batch_free(batch);

  m_history.push_back(result);
  return result;
}

std::string LlamacppBackend::buildPrompt() const {
  auto tmpls = common_chat_templates_init(m_model, "");

  common_chat_templates_inputs inputs;
  for (const auto &[role, content] : m_history) {
    common_chat_msg msg;
    msg.role = role;
    msg.content = content;
    inputs.messages.push_back(std::move(msg));
  }
  inputs.add_generation_prompt = true;
  inputs.use_jinja = true;

  auto params = common_chat_templates_apply(tmpls.get(), inputs);
  return params.prompt;
}

} // namespace LLM
