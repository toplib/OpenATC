#include "HttpClient.h"

#include <stdexcept>

namespace Net {

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    HttpClient::HttpClient() {
        m_curl = curl_easy_init();

        if (!m_curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        applyDefaultOptions();
    }

    HttpClient::~HttpClient() {
        if (m_curl) {
            curl_easy_cleanup(m_curl);
            m_curl = nullptr;
        }
    }

    Response HttpClient::get(const std::string& url, const std::vector<std::string>& headers) {
        if (!m_curl) {
            throw std::runtime_error("Failed to initalize CURL");
        }
        std::string responseBody;
        curl_easy_reset(m_curl);
        applyDefaultOptions();
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &responseBody);

        curl_slist* headerList = nullptr;

        for (const auto& header : headers) {
            headerList = curl_slist_append(headerList, header.c_str());
        }

        if (headerList) {
            curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headerList);
        }

        CURLcode res = curl_easy_perform(m_curl);

        if (headerList) {
            curl_slist_free_all(headerList);
        }

        if (res != CURLE_OK) {
            std::string error = curl_easy_strerror(res);
            throw std::runtime_error("Failed to perform get request: " + error);
        }

        long statusCode = 0;

        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &statusCode);

        return {.status = statusCode, .body = responseBody};
    }

    Response HttpClient::post(const std::string& url, const std::string& body, const std::vector<std::string>& headers) {
        if (!m_curl) {
            throw std::runtime_error("Failed to initalize CURL");
        }
        std::string responseBody;
        curl_easy_reset(m_curl);
        applyDefaultOptions();
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, body.size());

        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &responseBody);

        curl_slist* headerList = nullptr;

        for (const auto& header : headers) {
            headerList = curl_slist_append(headerList, header.c_str());
        }

        if (headerList) {
            curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headerList);
        }

        CURLcode res = curl_easy_perform(m_curl);

        if (headerList) {
            curl_slist_free_all(headerList);
        }

        if (res != CURLE_OK) {
            std::string error = curl_easy_strerror(res);
            throw std::runtime_error("Failed to perform post request: " + error);
        }

        long statusCode = 0;

        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &statusCode);

        return {.status = statusCode, .body = responseBody};
    }

    void HttpClient::applyDefaultOptions() {
        curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "SimpleATC/0.1");
    }
}
