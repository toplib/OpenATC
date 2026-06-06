#include "ATCPipeline.h"
#include "llm/Message.h"
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
      {{"system", "You are Pilot, you should answer exactly right if ATC asked "
                  "for. Please return ONLY plain text with phrase to ATC"}});
  LLM::Message response = m_llmBackend->getResponse({"user", command});

  m_llmBackend->setHistory({{"system", buildTtsPrompt()}});

  LLM::Message ttsResponse =
      m_llmBackend->getResponse({"user", response.content});
  if (m_responseCallback)
    m_responseCallback(ttsResponse.content);
}

void ATCPipeline::onResponse(
    std::function<void(std::string)> responseCallback) {
  m_responseCallback = responseCallback;
}
} // namespace Pipeline
