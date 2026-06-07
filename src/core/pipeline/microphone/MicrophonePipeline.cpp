#include "MicrophonePipeline.h"
#include "microphone/Microphone.h"
#include "webrtc_vad.h"
#include <spdlog/spdlog.h>

namespace Pipeline {

MicrophonePipeline::MicrophonePipeline(const MicrophonePipelineConfig config)
    : m_config(config), m_microphone(m_microphoneConfig) {
  m_vad = WebRtcVad_Create();
  WebRtcVad_Init(m_vad);
  WebRtcVad_set_mode(m_vad, m_config.vadMode);

  m_frame.reserve(m_config.frameSize);
  m_speechBuffer.reserve(16000 * 10);
}

MicrophonePipeline::~MicrophonePipeline() {
  stop();
  WebRtcVad_Free(m_vad);
}

MicrophoneInputAudioData *MicrophonePipeline::start() {
  spdlog::info("Starting capture data from microphone");

  m_audioData = m_microphone.start();

  m_thread = std::jthread([this](std::stop_token token) {
    while (!token.stop_requested()) {
      std::vector<float> samples;

      {
        std::lock_guard<std::mutex> lock(m_audioData->mutex);

        while (!m_audioData->samples.empty()) {
          samples.push_back(m_audioData->samples.front());
          m_audioData->samples.pop();
        }
      }

      if (samples.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        continue;
      }

      for (float sample : samples) {
        sample = std::clamp(sample, -1.0f, 1.0f);

        std::int16_t s16 = static_cast<std::int16_t>(sample * 32767.0f);

        m_frame.push_back(s16);

        if (m_frame.size() >= m_config.frameSize) {
          bool is_speech = WebRtcVad_Process(m_vad, 16000, m_frame.data(),
                                             m_config.frameSize) == 1;

          if (is_speech) {
            m_speaking = true;
            m_silenceFrames = 0;
            m_speechFrames++;

            m_speechBuffer.insert(m_speechBuffer.end(), m_frame.begin(),
                                  m_frame.end());
          } else if (m_speaking) {
            m_silenceFrames++;

            if (m_silenceFrames <= m_config.postSpeechPadFrames) {
              m_speechBuffer.insert(m_speechBuffer.end(), m_frame.begin(),
                                    m_frame.end());
            }

            if (m_silenceFrames > m_config.silenceFramesThreshold) {
              if (m_speechFrames >= m_config.minSpeechFrames &&
                  m_speechBuffer.size() >= 16000) {
                std::queue<float> speech;

                for (auto sample : m_speechBuffer) {
                  speech.push(sample / 32768.0f);
                }

                {
                  std::lock_guard<std::mutex> lock(m_result.mutex);

                  m_result.samples = std::move(speech);
                }

                m_speechBuffer.clear();
                m_speaking = false;
                m_silenceFrames = 0;
                m_speechFrames = 0;
              }
            }
          }

          m_frame.clear();
        }
      }
    }
  });

  return &m_result;
}

void MicrophonePipeline::stop() {
  m_thread.request_stop();
  m_microphone.stop();

  spdlog::info("Stopped capturing data from microphone");
}

} // namespace Pipeline
