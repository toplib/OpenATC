#include <spdlog/spdlog.h>

#include "microphone/Microphone.h"
#include "stt/Whisper/WhisperBackend.h"
#include <vad/include/webrtc_vad.h>

int main(int argc, char *argv[]) {
  spdlog::info("STT test");

  VadInst *vad = WebRtcVad_Create();
  WebRtcVad_Init(vad);
  WebRtcVad_set_mode(vad, 3);

  Input::MicrophoneConfig microphone_config;
  Input::Microphone microphone(microphone_config);

  STT::WhisperConfig whisper_config = {
      .model = "/home/toplib/CLionProjects/SimpleATC/models/"
               "ggml-large-v3-turbo-q8_0.bin"};
  STT::WhisperBackend backend(whisper_config);

  constexpr size_t frame_size = 320;
  constexpr int silence_frames_threshold = 15;
  constexpr int post_speech_pad_frames = 15;
  constexpr int min_speech_frames = 4;

  std::vector<std::int16_t> frame;
  frame.reserve(frame_size);

  std::vector<std::int16_t> speech_buffer;
  speech_buffer.reserve(16000 * 10);

  bool speaking = false;
  int silence_frames = 0;
  int speech_frames = 0;

  Input::AudioData *data = microphone.start();

  while (true) {
    std::vector<float> samples;

    {
      std::lock_guard<std::mutex> lock(data->mutex);

      while (!data->samples.empty()) {
        samples.push_back(data->samples.front());
        data->samples.pop();
      }
    }

    for (float sample : samples) {
      sample = std::clamp(sample, -1.0f, 1.0f);
      std::int16_t s16 = static_cast<std::int16_t>(sample * 32767.0f);

      frame.push_back(s16);

      if (frame.size() >= frame_size) {
        bool is_speech =
            WebRtcVad_Process(vad, 16000, frame.data(), frame_size) == 1;

        if (is_speech) {
          speaking = true;
          silence_frames = 0;
          speech_frames++;

          speech_buffer.insert(speech_buffer.end(), frame.begin(), frame.end());
        } else if (speaking) {
          silence_frames++;

          if (silence_frames <= post_speech_pad_frames) {
            speech_buffer.insert(speech_buffer.end(), frame.begin(),
                                 frame.end());
          }

          if (silence_frames > silence_frames_threshold) {
            if (speech_frames >= min_speech_frames &&
                speech_buffer.size() >= 16000) {
              std::span<const std::int16_t> span(speech_buffer);

              auto future = backend.transcribe(
                  span, {
                            R"(ATC Tower. ICAO English phraseology.
Multiple aircraft.
Numbers digit by digit: seven five eight one, two seven right, one three five decimal two zero.
Common controller instructions: cleared for takeoff, line up and wait, after departure contact departure, wind report.
Transcribe exactly what is spoken. No repetitions.)"});

              std::string text = future.get();

              if (!text.empty()) {
                spdlog::info("Transcription: {}", text);
              }
            }

            speech_buffer.clear();
            speaking = false;
            silence_frames = 0;
            speech_frames = 0;
          }
        }

        frame.clear();
      }
    }
  }

  WebRtcVad_Free(vad);

  return 0;
}
