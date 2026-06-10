#pragma once

#include <condition_variable>
#include <filesystem>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

#include "ai/stt/ISTTBackend.h"
#include "whisper.h"

struct whisper_context;

namespace STT {
struct WhisperConfig {
  std::filesystem::path model;
  std::string language = "en";
  bool translate = false;
  bool no_context = true;
  bool single_segment = true;
};

class WhisperBackend : public ISTTBackend {
public:
  WhisperBackend(WhisperConfig config);
  ~WhisperBackend() override;

  std::future<std::string> transcribe(std::span<const std::int16_t> &speech,
                                      Parameters parameters = {""}) override;
  void pushAudio(std::span<const std::int16_t> chunk) override;
  std::future<std::string> finalize() override;
  void reset() override;

private:
  struct Job {
    std::vector<std::int16_t> audio;
    std::string prompt;
    std::promise<std::string> promise;
  };

  void worker();

  WhisperConfig m_config;
  whisper_context *m_ctx = nullptr;
  whisper_full_params m_params;

  std::thread m_thread;
  std::queue<Job> m_queue;
  std::condition_variable m_cv;
  std::mutex m_mutex;
  bool m_stop = false;
};
} // namespace STT
