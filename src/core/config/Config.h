#pragma once
#include <filesystem>

namespace Configuration {
    class Config {
    protected:
        Config();
        ~Config();

        static Config* m_config;
    public:
        Config(Config& other) = delete;
        void operator=(const Config&) = delete;

        static Config* getInstance();

        void load();
        void save();

        int getMaxParallelDownloadingFiles();
        void setMaxParallelDownloadingFiles(int value);
    private:
        int maxParallelDownloadingFiles = 0;
    };
}
