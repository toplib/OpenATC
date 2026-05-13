#include <iostream>
// #include "net/HttpClient.h"
// #include <json.hpp>
// const std::string HF_DOWNLOADS_PATH = "./HF_DOWNLOADS/";
#include "core/stt-backend/Whisper/WhisperBackend.h"
#include "utils/WavLoader.h"

//using json = nlohmann::json;

int main() {
    // Net::HttpClient client;
    //
    // //Net::Response response = client.post("https://httpbin.org/post", R"({"hello": "world"})", {"Content-Type: application/json"});
    // Net::Response response = client.get("https://httpbin.org/get");
    // response = client.get(
    //         "https://huggingface.co/api/whoami-v2",
    //         {
    //             "Authorization: Bearer "
    //         }
    //         );
    // json data = json::parse(response.body);
    // if (data.contains("error")) {
    //     std::cerr << data["error"] << std::endl;
    // }
    // std::cout << "Code: " << response.status << std::endl;
    // std::cout << response.body << std::endl;

    STT::WhisperConfig config = {
        "/home/toplib/CLionProjects/SimpleATC/models/ggml-large-v3.bin",
        "en",
        false,
        true
    };

    std::vector<std::int16_t> vwav = loadWav("/home/toplib/CLionProjects/SimpleATC/output1.wav");
    std::span<const std::int16_t> wav(vwav);
    STT::WhisperBackend backend(config);
    std::cout << "Start transcribing" << std::endl;
    auto text = backend.transcribe(wav);
    while (text.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        std::cout << "Transcribing" << std::endl;
    }
    std::cout << text.get() << std::endl;
    return 0;
}
