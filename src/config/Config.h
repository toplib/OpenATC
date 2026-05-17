#pragma once
#include <filesystem>

namespace Configuration {
    struct SConfig { // S - Struct
        std::string languageCode;
        std::filesystem::path translationPath;

        // API Providers
        std::string openAIBaseApiAddress;
        std::string openAIAPIKey;
        std::string openAIModel;


    };
    class Config {
    public:
        Config();
        ~Config();

        SConfig load(std::filesystem::path& path);
        void save(std::filesystem::path& path, SConfig);
    };
}
