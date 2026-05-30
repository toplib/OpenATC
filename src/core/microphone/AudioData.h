#pragma once

#include <mutex>
#include <queue>

struct AudioData {
    std::mutex mutex;
    std::queue<float> samples;
};
