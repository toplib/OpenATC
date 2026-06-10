#include "WhisperBackend.h"

#include <vector>

namespace STT {
    WhisperBackend::WhisperBackend(WhisperConfig config) {
        m_config = config;
        m_ctx = whisper_init_from_file(m_config.model.c_str());

        if (!m_ctx) {
            throw std::runtime_error("Failed to load model");
        }

        m_params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

        m_params.print_progress   = false;
        m_params.print_realtime   = false;
        m_params.print_timestamps = false;

        m_params = whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH);
        m_params.strategy = WHISPER_SAMPLING_BEAM_SEARCH;
        m_params.beam_search.beam_size = 5;

        m_params.language       = m_config.language.c_str();
        m_params.translate      = m_config.translate;
        m_params.no_context     = m_config.no_context;
        m_params.single_segment = m_config.single_segment;

        m_params.temperature     = 0.0f;
        m_params.temperature_inc = 0.2f;

        m_params.no_speech_thold = 0.6f;
        m_params.logprob_thold   = -1.0f;
        m_params.entropy_thold   = 2.5f;

        m_params.audio_ctx = 768;

        m_params.suppress_blank = true;
        m_params.suppress_nst = true;

        m_thread = std::thread(&WhisperBackend::worker, this);
    }

    WhisperBackend::~WhisperBackend() {
        {
            std::lock_guard lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
        whisper_free(m_ctx);
    }

    std::future<std::string> WhisperBackend::transcribe(std::span<const std::int16_t>& speech, Parameters parameters) {
        Job job;
        job.audio.assign(speech.begin(), speech.end());
        job.prompt = parameters.prompt;

        std::future<std::string> future = job.promise.get_future();

        {
            std::lock_guard lock(m_mutex);
            m_queue.push(std::move(job));
        }
        m_cv.notify_one();

        return future;
    }

    void WhisperBackend::worker() {
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
                size_t samples = job.audio.size();

                std::vector<float> pcm_f32(samples);
                for (size_t i = 0; i < samples; i++) {
                    pcm_f32[i] = job.audio[i] / 32768.0f;
                }

                m_params.initial_prompt = job.prompt.c_str();

                if (whisper_full(m_ctx, m_params, pcm_f32.data(), pcm_f32.size()) != 0) {
                    throw std::runtime_error("Failed to process audio");
                }

                std::string buffer;
                int n_segments = whisper_full_n_segments(m_ctx);
                for (int i = 0; i < n_segments; i++) {
                    buffer += whisper_full_get_segment_text(m_ctx, i);
                }

                job.promise.set_value(std::move(buffer));
            } catch (...) {
                job.promise.set_exception(std::current_exception());
            }
        }
    }

    void WhisperBackend::pushAudio(std::span<const std::int16_t> chunk) {
    }

    std::future<std::string> WhisperBackend::finalize() {
        std::promise<std::string> p;
        p.set_value("");
        return p.get_future();
    }

    void WhisperBackend::reset() {
    }
}