#pragma once
#include <vector>

#include "Message.h"

namespace LLM {
    class ILLMBackend {
    public:
        virtual ~ILLMBackend() = default;

        virtual void setHistory(const std::vector<Message>& history) = 0;
        virtual const std::vector<Message>& getHistory() const = 0;

        virtual Message getResponse(const Message& message) = 0;
    };
}
