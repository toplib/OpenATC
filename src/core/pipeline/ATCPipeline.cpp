#include "ATCPipeline.h"

namespace Pipeline {
    ATCPipeline::ATCPipeline(const ATCPipelineConfig &config) : m_config(std::move(config)) {}
    ATCPipeline::~ATCPipeline() = default;

    void ATCPipeline::reloadConfig(const ATCPipelineConfig &config) {
        m_config = config;
    }
}
