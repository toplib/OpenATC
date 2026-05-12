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

    }

    WhisperBackend::~WhisperBackend() {
        whisper_free(m_ctx);
    }

    std::future<std::string> WhisperBackend::transcribe(std::span<const std::int16_t> &speech) {
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

        std::promise<std::string> p;
        p.set_value(buffer); // TODO: Implement transcribing in new thread
        return p.get_future();
    }

    void WhisperBackend::pushAudio(std::span<const std::int16_t> chunk) {
        // TODO: Implement streaming in future
    }

    std::future<std::string> WhisperBackend::finalize() {

    }

    void WhisperBackend::reset() {

    }
}
