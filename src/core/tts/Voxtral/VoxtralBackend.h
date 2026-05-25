#pragma once

#include <filesystem>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "voxtral.h"
#include "../ITTSBackend.h"

namespace TTS {

struct VoxtralConfig {
    std::filesystem::path model;
    std::filesystem::path tokenizer;
    std::filesystem::path voice;
};

class VoxtralBackend : public ITTSBackend {
public:
    explicit VoxtralBackend(VoxtralConfig config);
    ~VoxtralBackend() override;

    std::future<AudioOutput> speak(const std::string& text) override;

private:
    struct Job {
        std::string text;
        std::promise<AudioOutput> promise;
    };

    void worker();

    VoxtralConfig m_config;
    VoxtralTtsCtx* m_ctx = nullptr;

    std::jthread m_thread;
    std::queue<Job> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_stop = false;
};

}
