#include "ATCPipeline.h"
#include "ai/llm/Message.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <format>
#include <functional>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <vector>

namespace Pipeline {

static const std::vector<std::pair<std::string, std::string>> airlineCodes = {
    {"BRU", "Belavia"},        {"AFL", "Aeroflot"},
    {"SDM", "Rossiya"},        {"SBI", "S7"},
    {"LLM", "Ural"},           {"UTN", "Utair"},
    {"DLH", "Lufthansa"},      {"BAW", "Speedbird"},
    {"UAL", "United"},         {"DAL", "Delta"},
    {"AAL", "American"},       {"SWA", "Southwest"},
    {"AFR", "Air France"},     {"KLM", "KLM"},
    {"THY", "Turkish"},        {"CPA", "Cathay"},
    {"SIA", "Singapore"},      {"QTR", "Qatar"},
    {"UAE", "Emirates"},       {"ETD", "Etihad"},
    {"CES", "China Eastern"},  {"CCA", "Air China"},
    {"CSN", "China Southern"}, {"JAL", "Japan Air"},
    {"KAL", "Korean Air"},     {"ANZ", "Air New Zealand"},
    {"QFA", "Qantas"},         {"FIN", "Finnair"},
    {"SAS", "Scandinavian"},   {"RYR", "Ryanair"},
    {"EZY", "Easyjet"},
};

ATCPipeline::ATCPipeline(ATCPipelineConfig &config) {
  m_llmBackend = std::move(config.llmBackend);
}
ATCPipeline::~ATCPipeline() = default;

void ATCPipeline::start() {
  spdlog::info("Starting ATC Pipeline orchestrator");

  m_responseCallback = [](std::string text) {
    spdlog::warn("responseCallback is not set");
  };
}

static std::string buildTtsPrompt() {
  std::ostringstream prompt;
  prompt
      << "You convert ATC pilot text to TTS-ready speech with aviation "
         "phonetic rules.\n"
         "Return ONLY the converted text. NEVER add anything.\n"
         "\n"
         "Rules:\n"
         "- Keep regular English words as-is (wind, knots, cleared, takeoff, "
         "runway, etc.)\n"
         "- Convert each digit individually: 7581 -> seven five eight one, "
         "285 -> two eight five\n"
         "- Convert runway designators: 31R -> three one right, 31L -> "
         "three one left\n"
         "- Convert callsign letters to NATO: BRU -> Bravo Romeo Uniform\n"
         "- Convert waypoint names (uppercase letter groups like KODAP, UM, "
         "GOLIM) to NATO spelling: KODAP -> Kilo Oscar Delta Alpha "
         "Papa\n"
         "- Convert airway/track designators to NATO spelling: B145 -> "
         "Bravo one four five\n"
         "- Convert frequencies: 135.200 -> one three five decimal two "
         "zero zero\n"
         "- Convert headings: heading 270 -> heading two seven zero\n"
         "- Convert altitudes: FL350 -> flight level three five zero\n"
         "- Convert squawk: squawk 5732 -> squawk five seven three two\n"
         "- Keep punctuation: commas, periods, spaces as-is\n"
         "\n"
         "Airline callsigns (expand to full name before NATO conversion):\n";
  for (const auto &[code, name] : airlineCodes) {
    prompt << std::format("  {} - {}\n", code, name);
  }
  prompt << "\n"
            "Example: callsign \"BRU7581\" \u2192 say \"Belavia seven five "
            "eight one\" (expanded airline, then digits individually)\n"
            "Example: callsign \"AFL123\" \u2192 say \"Aeroflot one two "
            "three\"\n";
  return prompt.str();
}

void ATCPipeline::pushVoiceCommand(std::string command) {
  spdlog::info("Pushing voice command");
  m_llmBackend->setHistory(
      {{"system",
        R"(You are an aviation ATC (Air Traffic Control) transcription correction and validation engine.

Your ONLY task is to process noisy speech-to-text input from aviation communications.

You MUST follow these rules strictly:

1. Do NOT invent missing information.
2. Do NOT guess callsigns, aircraft numbers, frequencies, runways, or clearances.
3. Only correct what is clearly inferable from the input.
4. If any critical aviation element is unclear or unreliable, you MUST return status false.

Critical aviation elements include:
- Aircraft callsign (e.g., Belavia 7581)
- Runway identifiers (e.g., runway 27, 7R, 25L)
- Radio frequencies (e.g., 135.200)
- Clearances and instructions (takeoff, landing, climb, descent, contact instructions)

5. If the input is too corrupted, ambiguous, or partially unintelligible, return:
{"status": false}

6. If the input is valid enough to be corrected, return ONLY valid JSON in this format:

{"status": true, "message": "<corrected aviation phrase>"}

7. The "message" must:
- Follow standard ICAO ATC phraseology
- Correct obvious ASR mistakes (e.g. "created for takeoff" → "cleared for takeoff")
- Normalize numbers into spoken ICAO format:
  - 135.200 → one three five decimal two zero zero
- Remove artifacts like repeated dashes, broken digits, or mis-segmented numbers
- Preserve meaning exactly, do not add new instructions

8. If frequency or callsign is partially unreadable or inconsistent, return status false.

9. Output MUST be valid JSON only. No explanations. No extra text. No formatting.

Examples:

Input: "Belavia 7581 cleared for takeoff runway 27 contact departure 135.200"
Output:
{"status": true, "message": "Belavia seven five eight one cleared for takeoff runway two seven contact departure one three five decimal two zero zero"}

Input: "Bela 7 something cleared takeoff 27 contact departure"
Output:
{"status": false}

You are a strict safety-critical aviation transcription validator.)"}});
  LLM::Message fixedMessage = m_llmBackend->getResponse({"user", command});

  json j = json::parse(fixedMessage.content);

  if (j.contains("status") && j["status"] != false) {
    spdlog::info(std::format("Fixed: {}", std::string(j["message"])));
    m_llmBackend->setHistory(
        {{"system",
          "You are Pilot, you should answer exactly right if ATC asked "
          "for. Please return ONLY plain text with phrase to ATC"}});
    LLM::Message response = m_llmBackend->getResponse({"user", j["message"]});

    m_llmBackend->setHistory({{"system", buildTtsPrompt()}});

    LLM::Message ttsResponse =
        m_llmBackend->getResponse({"user", response.content});
    if (m_responseCallback)
      m_responseCallback(ttsResponse.content);
  } else {
    spdlog::warn("Cannot recognize message, skipping");
  }
}

void ATCPipeline::onResponse(
    std::function<void(std::string)> responseCallback) {
  m_responseCallback = responseCallback;
}
} // namespace Pipeline
