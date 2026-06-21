#include <cassert>
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

#include "ai/llm/llamacpp/LlamacppBackend.h"
#include "ai/stt/Whisper/WhisperBackend.h"
#include "ai/tts/SherpaOnnx/SherpaOnnxBackend.h"
#include "audio/OpenAL/OpenALBackend.h"
#include "audio/Sound.h"
#include "gui/Manager.h"
#include "pipeline/ATCPipeline.h"
#include "pipeline/microphone/MicrophonePipeline.h"

int main(int argc, char *argv[]) {

  Pipeline::MicrophonePipeline microphonePipeline({});
  STT::WhisperBackend whisper(
      {.model = "./../models/ggml-large-v3-turbo-q8_0.bin"});

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
    spdlog::info("Audio generated");
    audio.pushAudioFrame(speech);
  });
  MicrophoneInputAudioData *data = microphonePipeline.start();

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
    std::future<std::string> future =
        whisper.transcribe(span, {
                                     R"(ATC Tower. ICAO English phraseology.
Multiple aircraft.
Numbers digit by digit: seven five eight one, two seven right, one three five decimal two zero.
Common controller instructions: cleared for takeoff, line up and wait, after departure contact departure, wind report.
Transcribe exactly what is spoken. No repetitions.)"});
    std::string result = future.get();
    spdlog::info(std::format("Transcribed: {}", result));
    pipeline.pushVoiceCommand(result);
  }

  GUI::Manager manager(argc, argv);
  int result = manager.run();

  return result;
}
