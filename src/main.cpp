#include <iostream>

#include "llm-backend/llamacpp/LlamacppBackend.h"

int main() {
    LLM::LlamacppConfig config = {
        "/home/toplib/CLionProjects/SimpleATC/models/llama-3.2-1b-instruct-q8_0.gguf",
        4096,
        512,
        8,
        true,
        200
    };
    LLM::LlamacppBackend backend(config);
    std::vector<LLM::Message> history = backend.getHistory();
    history.push_back({"system", "You're pilot of the Airliner called Boeing 737-800, you will communicate with ATC, your callsign is BRU7581, BRU - Belavia, ANSWER EXACTLY WHAT YOU WILL SAY TO ATC, WITH ICAO STANDARDS"});
    backend.setHistory(history);
    std::future<LLM::Message> future = backend.getResponse({"user", "BRU7581, cleared for takeoff, runway 27R, wind 085 at 15 knots, after departure contact departure 135.200"});
    // std::future<LLM::Message> future = backend.getResponse({"user", "Write hello world in c++"});
    std::cout << "WARNING: STARTING GENERATION" << std::endl;
    std::cout << future.get().content << std::endl;
    return 0;
}
