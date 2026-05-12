#include "HuggingFace.h"

#include "net/HttpClient.h"

namespace HF {
    HuggingFace::HuggingFace(const std::string &hf_token) {
        m_token = hf_token;
    }

    HuggingFace::~HuggingFace() {
        for (auto& t : m_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void HuggingFace::setToken(const std::string& hf_token) {
        m_token = hf_token;
    }

    bool HuggingFace::isValidToken() {
        Net::HttpClient client;
        client.get(
            "https://huggingface.co/api/whoami-v2",
            {
                "Authorization: Bearer " + m_token
            }
            );
    	return false;
    }
}
