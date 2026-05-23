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

        m_params.print_progress = false;
        m_params.print_realtime = false;
        m_params.print_timestamps = false;

        m_params.language = m_config.language.c_str();
        m_params.translate = m_config.translate;
        m_params.no_context = m_config.no_context;
        m_params.single_segment = m_config.single_segment;

    }

    WhisperBackend::~WhisperBackend() {
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
        whisper_free(m_ctx);
    }

    std::future<std::string> WhisperBackend::transcribe(std::span<const std::int16_t>& speech, Parameters parameters) {
        if (m_thread.joinable()) {
            m_thread.join();
        }
        auto promise = std::make_shared<std::promise<std::string>>();
        std::future<std::string> future = promise->get_future();
        m_params.initial_prompt = parameters.prompt.c_str();

        m_thread = std::thread([this, promise, speech]() {
            try {
                size_t samples = speech.size();

                std::vector<float> pcm_f32(samples);
                for (size_t i = 0; i < samples; i++) {
                    pcm_f32[i] = speech[i] / 32768.0f;
                }

                if (whisper_full(m_ctx, m_params, pcm_f32.data(), pcm_f32.size()) != 0) {
                    whisper_free(m_ctx);
                    throw std::runtime_error("Failed to process audio");
                }

                std::string buffer = "";
                int n_segments = whisper_full_n_segments(m_ctx);
                for (int i = 0; i < n_segments; i++) {
                    const char* text = whisper_full_get_segment_text(m_ctx, i);
                    buffer += text;
                }
                promise->set_value(buffer);
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        });

        return future;
    }

    void worker() {

    }

    void WhisperBackend::pushAudio(std::span<const std::int16_t> chunk) {
        // TODO: Implement streaming in future
    }

    std::future<std::string> WhisperBackend::finalize() {
        std::promise<std::string> p;
        p.set_value("");
        return p.get_future();
    }

    void WhisperBackend::reset() {

    }
}
