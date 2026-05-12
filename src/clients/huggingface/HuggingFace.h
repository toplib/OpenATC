#pragma once
#include <string>
#include <filesystem>
#include <functional>
#include <vector>
#include <thread>

#include "utils/FileNode.h"

namespace HF {
    class HuggingFace {
    public:
        HuggingFace(const std::string& hf_token);
        ~HuggingFace();

        void setToken(const std::string& hf_token);
        bool isValidToken();

        void setDownloadPath(std::filesystem::path path);

        // Repo management
        void setRepo(const std::string& url); // Accept only author/repo
        bool isValidRepo();
        std::vector<std::string> list();
        FileNode recursiveList();
        void setThreadLimit(int threads);
        // Single thread only
        void download(); // Will create folder something like this "./author/repo-name/" in download dir
        // Multi-thread
        void threadDownload(); // Same but with threading

        bool isCached(const std::filesystem::path& filename); // Will check for file availability and SHA-256
        // Single thread only
        void downloadFile(const std::filesystem::path& filepath);
        // Multi-thread
        void threadDownloadFile(const std::filesystem::path& filepath);

        using ProgressCallback = std::function<void(const std::string& filename, size_t downloaded, size_t total)>;
        void setProgressCallback(ProgressCallback cb);
        std::filesystem::path ensure(const std::filesystem::path& filename);
    private:
        std::string m_author;
        std::string m_repo;

        std::filesystem::path m_path = "./HF_DOWNLOADS/";
        std::string m_token = "";
        int m_threadCount = 4;
        ProgressCallback m_progress_cb;
        std::vector<std::thread> m_threads;
    };

    struct ChunkTask {
        std::string url;
        std::string hf_token;
        size_t offset_start;
        size_t offset_end;
        int fd;
        std::atomic<size_t>* atomic;
    };

    namespace Worker {
        void downloadChunk(ChunkTask task);
    }
}