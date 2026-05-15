#pragma once
#include <future>
#include <vector>

#include "Message.h"

namespace LLM {
    class ILLMBackend {
    public:
        virtual ~ILLMBackend() = default;

        virtual void setHistory(const std::vector<Message>& history) = 0;
        virtual std::vector<Message>& getHistory() = 0;

        virtual std::future<Message> getResponse(const Message& message) = 0;
    };
}
