#pragma once

#include "audio/IAudioBackend.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace Audio {
class OpenALBackend : public IAudioBackend {
public:
  OpenALBackend();
  ~OpenALBackend() override;

  OpenALBackend(const OpenALBackend &) = delete;
  OpenALBackend &operator=(const OpenALBackend &) = delete;

  void pushAudioFrame(const Sound &sound) override;
  void play() override;
  void stop();

private:
  bool initDevice();
  void shutdownDevice();
  bool playSound(const Sound &sound);
  void workerLoop();

  static std::vector<int16_t> convertToInt16(const std::vector<float> &samples);
  static void checkAlError(const char *label);

  ALCdevice *m_device = nullptr;
  ALCcontext *m_context = nullptr;

  std::thread m_workerThread;
  std::atomic<bool> m_running{false};
  std::atomic<bool> m_stopRequested{false};

  mutable std::mutex m_queueMutex;
  std::condition_variable m_queueCv;
  std::queue<Sound> m_soundQueue;
};
} // namespace Audio
