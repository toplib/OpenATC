#include <cassert>
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

#include "audio/OpenAL/OpenALBackend.h"
#include "audio/Sound.h"
#include "gui/Manager.h"
#include "llm/llamacpp/LlamacppBackend.h"
#include "pipeline/ATCPipeline.h"
#include "pipeline/microphone/MicrophonePipeline.h"
#include "stt/Whisper/WhisperBackend.h"
#include "tts/SherpaOnnx/SherpaOnnxBackend.h"

int main(int argc, char *argv[]) {

  Pipeline::MicrophonePipeline microphonePipeline({});
  MicrophoneInputAudioData *data = microphonePipeline.start();

  STT::WhisperBackend whisper(
      {.model = "./../models/ggml-large-v3-turbo-q8_0.bin"});

  while (true) {
    std::vector<float> samples;

    {
      std::lock_guard<std::mutex> lock(data->mutex);

      while (!data->samples.empty()) {
        samples.push_back(data->samples.front());
        data->samples.pop();
      }
    }

    if (samples.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    std::vector<std::int16_t> speech;
    speech.reserve(samples.size());
    for (float s : samples) {
      s = std::clamp(s, -1.0f, 1.0f);
      speech.push_back(static_cast<std::int16_t>(s * 32767.0f));
    }

    std::span<const std::int16_t> span(speech);
    std::future<std::string> future = whisper.transcribe(span);
    spdlog::info(std::format("Transcribed: {}", future.get()));
  }
  TTS::SherpaOnnxKokoroConfig ttsConfig{
      .model = "../models/kokoro-en-v0_19/model.onnx",
      .voices = "../models/kokoro-en-v0_19/voices.bin",
      .tokens = "../models/kokoro-en-v0_19/tokens.txt",
      .dataDir = "../models/kokoro-en-v0_19/espeak-ng-data",
      .numThreads = 2,
      .debug = false};
  TTS::SherpaOnnxBackend tts(ttsConfig);

  Audio::OpenALBackend audio;
  audio.play();

  LLM::LlamacppConfig llamaConfig = {
      .model = "/run/media/toplib/Новый "
               "том/llms/lmstudio-community/gemma-4-E2B-it-GGUF/"
               "gemma-4-E2B-it-Q4_K_M.gguf",
      .contextSize = 2048,
      .batchSize = 512,
      .threads = 8,
      .generateUntilEOS = false,
      .tokenLimit = 200};
  auto llamacppBackend = std::make_unique<LLM::LlamacppBackend>(llamaConfig);

  Pipeline::ATCPipelineConfig pipelineConfig;
  pipelineConfig.llmBackend = std::move(llamacppBackend);

  Pipeline::ATCPipeline pipeline(pipelineConfig);
  pipeline.onResponse([&tts, &audio](std::string text) {
    spdlog::info(std::format("Speaking: \"{}\"", text));
    Sound speech = tts.speak(text);
    audio.pushAudioFrame(speech);
  });
  pipeline.pushVoiceCommand(
      "BRU7581, cleared for takeoff runway 27R, wind 235 at 15 knots, after "
      "departure contact departure 135.200, and have a nice flight");

  GUI::Manager manager(argc, argv);
  int result = manager.run();

  return result;
}
