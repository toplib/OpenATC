#pragma once
#include <functional>
#include <string>
#include <curl/curl.h>

namespace Net {
    struct Response {
        long status = 0;
        std::string body;
        std::vector<std::string> headers;
    };

    using ProgressCallback = std::function<void(size_t current, size_t total)>;

    class HttpClient {
    public:
        HttpClient();
        ~HttpClient();

        Response get(const std::string& url, const std::vector<std::string>& headers = {});
        Response post(const std::string& url, const std::string& body, const std::vector<std::string>& headers = {});

        void applyDefaultOptions();
    private:
        CURL* m_curl = nullptr;
    };
}
