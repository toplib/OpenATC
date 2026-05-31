#pragma once
#include <filesystem>

#include "sherpa-onnx/c-api/cxx-api.h"
#include "tts/ITTSBackend.h"

// TODO: Make other TTS available

namespace TTS {
struct SherpaOnnxKokoroConfig {
  std::filesystem::path model;
  std::filesystem::path voices;
  std::filesystem::path tokens;
  std::filesystem::path dataDir;

  int numThreads;
  bool debug;
}; // TODO: Make unified parameters
class SherpaOnnxBackend : public ITTSBackend {
public:
  explicit SherpaOnnxBackend(const SherpaOnnxKokoroConfig &config);
  ~SherpaOnnxBackend() override;

  Sound speak(const std::string &text) override;

private:
  SherpaOnnxKokoroConfig m_config;
  sherpa_onnx::cxx::OfflineTts m_tts;
};
} // namespace TTS
