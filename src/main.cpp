#include <iostream>
#include "net/HttpClient.h"
#include <json.hpp>
const std::string HF_DOWNLOADS_PATH = "./HF_DOWNLOADS/";

using json = nlohmann::json;

int main() {
    Net::HttpClient client;

    //Net::Response response = client.post("https://httpbin.org/post", R"({"hello": "world"})", {"Content-Type: application/json"});
    Net::Response response = client.get("https://httpbin.org/get");
    response = client.get(
            "https://huggingface.co/api/whoami-v2",
            {
                "Authorization: Bearer "
            }
            );
    json data = json::parse(response.body);
    if (data.contains("error")) {
        std::cerr << data["error"] << std::endl;
    }
    std::cout << "Code: " << response.status << std::endl;
    std::cout << response.body << std::endl;
    return 0;
}