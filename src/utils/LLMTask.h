#pragma once

#include "LLMTaskType.h"
#include <string>

struct LLMTask {
  LLMTaskType type;
  std::string arguments;
};
