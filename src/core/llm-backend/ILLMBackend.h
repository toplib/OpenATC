#pragma once
#include <future>
#include <vector>

#include "Message.h"

namespace LLM {
    class ILLMBackend {
    public:
        virtual ~ILLMBackend() = 0;

        virtual void setHistory(const std::vector<Message>& history);
        virtual std::vector<Message>& getHistory();

        virtual std::future<Message> getResponse(const Message& message);
    };
}
