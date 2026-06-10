#pragma once
#include <future>
#include <span>

namespace STT {
    struct Parameters {
        std::string prompt;
    };
    class ISTTBackend {
    public:
        virtual ~ISTTBackend() = default;

        virtual std::future<std::string> transcribe(std::span<const std::int16_t>& speech, Parameters parameters = {""}) = 0;

        virtual void pushAudio(std::span<const std::int16_t> chunk) = 0;
        virtual std::future<std::string> finalize() = 0;
        virtual void reset() = 0;
    };
}
