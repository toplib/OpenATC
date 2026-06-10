#pragma once

#include "ai/llm/ILLMBackend.h"
#include <functional>
#include <memory>
#include <string>

namespace Pipeline {

enum class CommandType {
  // Ground / Taxi
  TAXI_TO,
  TAXI_VIA,
  TAXI_TO_GATE,
  TAXI_TO_RAMP,
  TAXI_TO_HANGAR,
  PUSHBACK,
  START_ENGINES,
  HOLD_POSITION,
  HOLD_SHORT,
  STOP_TAXI,
  BACK_TAXI,
  CROSS_RUNWAY,
  EXPEDITE,

  // Takeoff / Departure
  CLEARED_FOR_TAKEOFF,
  LINE_UP_AND_WAIT,
  CANCEL_TAKEOFF_CLEARANCE,
  CLEARED_IMMEDIATE_TAKEOFF,

  // Landing / Approach
  CLEARED_TO_LAND,
  CLEARED_APPROACH,
  CLEARED_ILS_APPROACH,
  CLEARED_VOR_APPROACH,
  CLEARED_VISUAL_APPROACH,
  CLEARED_RNAV_APPROACH,
  GO_AROUND,
  LOW_APPROACH,
  TOUCH_AND_GO,
  FULL_STOP,

  // Heading / Navigation
  TURN_LEFT,
  TURN_RIGHT,
  FLY_HEADING,
  DIRECT_TO,
  HOLD,
  REPORT_POSITION,

  // Altitude / Vertical
  CLIMB_TO,
  DESCEND_TO,
  MAINTAIN_ALTITUDE,

  // Speed
  INCREASE_SPEED,
  REDUCE_SPEED,
  MAINTAIN_SPEED,

  // Communication / Transponder
  CONTACT,
  SQUAWK,
  SQUAWK_IDENT,
  SQUAWK_VFR,
  SET_ALTIMETER,

  // General / Advisory
  REPORT,
  ACKNOWLEDGE,
  SAY_AGAIN,
  STAND_BY,
  CHECK_GEAR,
  MAINTAIN,
  ROGER,
};

struct Command {
  CommandType type;
  std::string arguments;
};

struct ATCPipelineConfig {
  std::unique_ptr<LLM::ILLMBackend> llmBackend;
};

class ATCPipeline {
public:
  ATCPipeline(ATCPipelineConfig &config);
  ~ATCPipeline();

  void start();
  void pushVoiceCommand(std::string text);

  void onResponse(std::function<void(std::string)> responseCallback);

private:
  std::unique_ptr<LLM::ILLMBackend> m_llmBackend;

  std::function<void(std::string)> m_responseCallback;
  std::vector<Command> m_commandsHistory;
};
} // namespace Pipeline
