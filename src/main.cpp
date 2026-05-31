#include "gui/Manager.h"
#include <spdlog/spdlog.h>

#include "audio/OpenAL/OpenALBackend.h"
#include "audio/Sound.h"
#include "tts/SherpaOnnx/SherpaOnnxBackend.h"

int main(int argc, char *argv[]) {
  TTS::SherpaOnnxKokoroConfig config{
      .model = "../models/kokoro-en-v0_19/model.onnx",
      .voices = "../models/kokoro-en-v0_19/voices.bin",
      .tokens = "../models/kokoro-en-v0_19/tokens.txt",
      .dataDir = "../models/kokoro-en-v0_19/espeak-ng-data",
      .numThreads = 2,
      .debug = false};
  TTS::SherpaOnnxBackend tts(config);

  Audio::OpenALBackend audio;
  audio.play();

  Sound speech = tts.speak(
      "Cleared for takeoff, runway two seven romeo, after departure contact "
      "departure one three five decimal two zero zero");

  audio.pushAudioFrame(speech);

  // audio won't block main thread — GUI starts immediately
  // playback continues in the background

  GUI::Manager manager(argc, argv);
  int result = manager.run();

  // stop ensures all queued audio finishes and cleans up
  audio.stop();

  return result;
}
