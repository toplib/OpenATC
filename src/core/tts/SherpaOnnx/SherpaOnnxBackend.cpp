#include "SherpaOnnxBackend.h"

#include <spdlog/spdlog.h>

namespace TTS {
SherpaOnnxBackend::SherpaOnnxBackend(const SherpaOnnxKokoroConfig &config)
    : m_config(config), m_tts(sherpa_onnx::cxx::OfflineTts::Create(
                            [](const SherpaOnnxKokoroConfig &cfg) {
                              sherpa_onnx::cxx::OfflineTtsConfig ttsConfig;
                              ttsConfig.model.kokoro.model = cfg.model;
                              ttsConfig.model.kokoro.voices = cfg.voices;
                              ttsConfig.model.kokoro.tokens = cfg.tokens;
                              ttsConfig.model.kokoro.data_dir = cfg.dataDir;
                              ttsConfig.model.num_threads = cfg.numThreads;
                              ttsConfig.model.debug = cfg.debug;
                              return ttsConfig;
                            }(config))) {}

SherpaOnnxBackend::~SherpaOnnxBackend() = default;

Sound SherpaOnnxBackend::speak(const std::string &text) {
  if (!m_tts.Get()) {
    throw std::runtime_error("TTS::OnnxRuntime::Get() failed");
  }

  spdlog::debug("TTS::OnnxRuntime::Generate()");

  auto audio = m_tts.Generate(text);
  if (audio.samples.empty()) {
    throw std::runtime_error(
        "TTS::OnnxRuntime::Generate()::audio.samples is empty");
  }

  return {audio.samples, audio.sample_rate};
}
} // namespace TTS
