#include "Config.h"

namespace Configuration {
    Config::Config() = default;
    Config::~Config() = default;

    Config* Config::getInstance() {
        if (!m_config) {
            m_config = new Config();
        }
        return m_config;
    }

    void load() {

    }
}
