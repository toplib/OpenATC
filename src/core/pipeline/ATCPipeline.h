#pragma once

#include "audio/IAudioBackend.h"
#include "microphone/Microphone.h"
#include "microphone/MicrophoneInputAudioData.h"
#include "utils/MicrophoneActivationType.h"
#include <functional>
#include <mutex>
#include <thread>

#include "ATCEventType.h"
#include "llm/ILLMBackend.h"
#include "stt/ISTTBackend.h"
#include "tts/ITTSBackend.h"
#include "utils/NoiseSuppressionType.h"
#include "webrtc_vad.h"

namespace Pipeline {
struct ATCPipelineConfig {
  MicrophoneActivationType microphoneActivationType;
  NoiseSuppressionType
      noiseSuppressionType; // TODO: Implement noise suppression

  std::function<void(ATCEventType, std::string_view)> onEventCallback;

  std::unique_ptr<LLM::ILLMBackend> llmBackend;
  std::unique_ptr<STT::ISTTBackend> sttBackend;
  std::unique_ptr<TTS::ITTSBackend> ttsBackend;

  std::unique_ptr<Input::Microphone> microphone;
  std::unique_ptr<Audio::IAudioBackend> audioBackend;

  size_t voiceFrameSize;
  int voiceSilenceFramesThreshold;
  int voicePostSpeechPadFrames;
  int voiceMinSpeechFrame;
};

class ATCPipeline {
public:
  ATCPipeline(ATCPipelineConfig config);
  ~ATCPipeline();

  void start();
  void stop();

private:
  void workerThread(std::stop_token token);

  ATCPipelineConfig m_config;
  std::jthread m_thread;
  std::mutex m_mutex;

  // Microphone pipeline
  MicrophoneInputAudioData *m_audioData = nullptr;
  VadInst *m_vad = nullptr;
  bool m_isSpeaking;
  std::vector<std::int16_t> m_frame;
  std::vector<std::int16_t> m_speechBuffer;
  int m_silenceFrames = 0;
  int m_speechFrames = 0;
  std::mutex m_microphoneMutex;
  void processVAD();
};
} // namespace Pipeline
