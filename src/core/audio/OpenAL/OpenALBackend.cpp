#include "OpenALBackend.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <algorithm>
#include <chrono>
#include <spdlog/spdlog.h>

namespace Audio {

OpenALBackend::OpenALBackend() {}

OpenALBackend::~OpenALBackend() { stop(); }

void OpenALBackend::pushAudioFrame(const Sound &audio) {
  {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_soundQueue.push(audio);
  }
  m_queueCv.notify_one();
}

void OpenALBackend::play() {
  if (m_running.load())
    return;

  if (!initDevice()) {
    spdlog::error("Failed to init OpenAL device");
    return;
  }

  m_stopRequested.store(false);
  m_running.store(true);
  m_workerThread = std::thread(&OpenALBackend::workerLoop, this);
}

void OpenALBackend::stop() {
  if (!m_running.load())
    return;

  m_stopRequested.store(true);
  m_queueCv.notify_all();

  if (m_workerThread.joinable())
    m_workerThread.join();

  shutdownDevice();
  m_running.store(false);
}

void OpenALBackend::workerLoop() {
  while (true) {
    Sound sound;

    {
      std::unique_lock<std::mutex> lock(m_queueMutex);
      m_queueCv.wait(lock, [this] {
        return !m_soundQueue.empty() || m_stopRequested.load();
      });

      if (m_soundQueue.empty() && m_stopRequested.load())
        break;

      if (m_soundQueue.empty())
        continue;

      sound = std::move(m_soundQueue.front());
      m_soundQueue.pop();
    }

    playSound(sound);
  }
}

bool OpenALBackend::initDevice() {
  m_device = alcOpenDevice(nullptr);
  if (!m_device) {
    spdlog::error("Failed to open OpenAL device");
    return false;
  }

  m_context = alcCreateContext(m_device, nullptr);
  if (!m_context) {
    spdlog::error("Failed to create OpenAL context");
    alcCloseDevice(m_device);
    m_device = nullptr;
    return false;
  }

  alcMakeContextCurrent(m_context);
  checkAlError("makeContextCurrent");

  alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
  alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alListenerf(AL_GAIN, 1.0f);

  return true;
}

void OpenALBackend::shutdownDevice() {
  alcMakeContextCurrent(nullptr);

  if (m_context) {
    alcDestroyContext(m_context);
    m_context = nullptr;
  }

  if (m_device) {
    alcCloseDevice(m_device);
    m_device = nullptr;
  }
}

bool OpenALBackend::playSound(const Sound &sound) {
  if (sound.samples.empty())
    return false;

  ALuint buffer, source;
  alGenBuffers(1, &buffer);
  checkAlError("genBuffers");
  alGenSources(1, &source);
  checkAlError("genSources");

  std::vector<int16_t> pcmData = convertToInt16(sound.samples);

  alBufferData(buffer, AL_FORMAT_MONO16, pcmData.data(),
               static_cast<ALsizei>(pcmData.size() * sizeof(int16_t)),
               sound.sampleRate);
  checkAlError("bufferData");

  alSourcef(source, AL_GAIN, 1.0f);
  alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcei(source, AL_BUFFER, buffer);
  checkAlError("sourcei buffer");

  alSourcePlay(source);
  checkAlError("sourcePlay");

  ALint state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  if (state != AL_PLAYING) {
    spdlog::error("Source failed to start playing");
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    return false;
  }

  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    if (m_stopRequested.load()) {
      alSourceStop(source);
      break;
    }
  } while (state == AL_PLAYING);

  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);

  return true;
}

std::vector<int16_t>
OpenALBackend::convertToInt16(const std::vector<float> &samples) {
  std::vector<int16_t> pcmData(samples.size());
  for (size_t i = 0; i < samples.size(); ++i) {
    float sample = std::clamp(samples[i], -1.0f, 1.0f);
    pcmData[i] = static_cast<int16_t>(sample * 32767.0f);
  }
  return pcmData;
}

void OpenALBackend::checkAlError(const char *label) {
  ALenum err = alGetError();
  if (err != AL_NO_ERROR)
    spdlog::error("OpenAL error after {}: {}", label, err);
}

} // namespace Audio
