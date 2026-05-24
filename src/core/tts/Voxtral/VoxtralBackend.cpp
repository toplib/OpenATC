#include "VoxtralBackend.h"

#include <stdexcept>

namespace TTS {

VoxtralBackend::VoxtralBackend(VoxtralConfig config) {
    m_config = std::move(config);
}

VoxtralBackend::~VoxtralBackend() {
    {
        std::lock_guard lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    if (m_thread.joinable()) {
        m_thread.join();
    }
    if (m_ctx) {
        voxtral_tts_destroy(m_ctx);
    }
}

void VoxtralBackend::load() {
    if (m_ctx) {
        voxtral_tts_destroy(m_ctx);
        m_ctx = nullptr;
    }

    m_ctx = voxtral_tts_create();
    if (!m_ctx) {
        throw std::runtime_error("Failed to create Voxtral TTS context");
    }

    auto modelStr = m_config.model.string();
    if (voxtral_tts_load_model(m_ctx, modelStr.c_str()) != 0) {
        std::string err = "Failed to load TTS model";
        const char* lastErr = voxtral_tts_last_error(m_ctx);
        if (lastErr) {
            err += ": ";
            err += lastErr;
        }
        voxtral_tts_destroy(m_ctx);
        m_ctx = nullptr;
        throw std::runtime_error(err);
    }

    auto tokenizerStr = m_config.tokenizer.string();
    if (voxtral_tts_load_tokenizer(m_ctx, tokenizerStr.c_str()) != 0) {
        std::string err = "Failed to load TTS tokenizer";
        const char* lastErr = voxtral_tts_last_error(m_ctx);
        if (lastErr) {
            err += ": ";
            err += lastErr;
        }
        voxtral_tts_destroy(m_ctx);
        m_ctx = nullptr;
        throw std::runtime_error(err);
    }

    auto voiceStr = m_config.voice.string();
    if (voxtral_tts_load_voice(m_ctx, voiceStr.c_str()) != 0) {
        std::string err = "Failed to load TTS voice";
        const char* lastErr = voxtral_tts_last_error(m_ctx);
        if (lastErr) {
            err += ": ";
            err += lastErr;
        }
        voxtral_tts_destroy(m_ctx);
        m_ctx = nullptr;
        throw std::runtime_error(err);
    }

    m_thread = std::jthread(&VoxtralBackend::worker, this);
}

std::future<AudioOutput> VoxtralBackend::speak(const std::string& text) {
    Job job;
    job.text = text;

    std::future<AudioOutput> future = job.promise.get_future();

    {
        std::lock_guard lock(m_mutex);
        m_queue.push(std::move(job));
    }
    m_cv.notify_one();

    return future;
}

void VoxtralBackend::worker() {
    while (true) {
        Job job;

        {
            std::unique_lock lock(m_mutex);
            m_cv.wait(lock, [this] { return m_stop || !m_queue.empty(); });

            if (m_stop && m_queue.empty()) {
                return;
            }

            job = std::move(m_queue.front());
            m_queue.pop();
        }

        try {
            if (!m_ctx) {
                throw std::runtime_error("TTS not loaded. Call load() first.");
            }

            float* samples = nullptr;
            int numSamples = 0;
            int sampleRate = 0;

            int ret = voxtral_tts_speak(m_ctx, job.text.c_str(), &samples, &numSamples, &sampleRate);

            if (ret != 0) {
                std::string err = "TTS synthesis failed";
                const char* lastErr = voxtral_tts_last_error(m_ctx);
                if (lastErr) {
                    err += ": ";
                    err += lastErr;
                }
                throw std::runtime_error(err);
            }

            AudioOutput output;
            output.sampleRate = sampleRate;

            if (samples && numSamples > 0) {
                output.samples.assign(samples, samples + numSamples);
                voxtral_tts_free_audio(samples, numSamples);
            }

            job.promise.set_value(std::move(output));
        } catch (...) {
            job.promise.set_exception(std::current_exception());
        }
    }
}

}
