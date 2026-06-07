#pragma once
#include "core/microphone/MicrophoneInputAudioData.h"
#include "microphone/Microphone.h"
#include "utils/MicrophoneActivationType.h"
#include "utils/NoiseSuppressionType.h"
#include "webrtc_vad.h"
#include <thread>

// TODO: Make VAD adapter class and move VAD to config via std::unique_ptr
// TODO: Make threading separated from MicrophonePipeline

namespace Pipeline {
struct MicrophonePipelineConfig {
  NoiseSuppressionType noiseSuppressionType =
      NoiseSuppressionType::NO_NOISE_SUPPRESSION;
  MicrophoneActivationType microphoneActivationType =
      MicrophoneActivationType::VOICE_ACTIVITY;
  size_t frameSize = 320;
  int silenceFramesThreshold = 15;
  int postSpeechPadFrames = 15;
  int minSpeechFrames = 4;
  int vadMode = 3;
};
class MicrophonePipeline {
public:
  MicrophonePipeline(const MicrophonePipelineConfig config);
  ~MicrophonePipeline();

  MicrophoneInputAudioData *start();
  void stop();

private:
  std::jthread m_thread;

  MicrophoneInputAudioData *m_audioData;
  MicrophoneInputAudioData m_result = {};

  MicrophonePipelineConfig m_config;
  VadInst *m_vad = nullptr;
  Input::MicrophoneConfig m_microphoneConfig;
  Input::Microphone m_microphone;

  std::vector<int16_t> m_frame;
  std::vector<int16_t> m_speechBuffer;

  bool m_speaking = false;
  int m_silenceFrames = 0;
  int m_speechFrames = 0;
};
} // namespace Pipeline
