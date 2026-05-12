#pragma once
#include <cstdint>
#include <ios>
#include <string>
#include <vector>
#include <fstream>

std::vector<std::int16_t> loadWav(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Cannot open file");
    }

    file.seekg(44);
    std::vector<std::int16_t> samples;

    int16_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        samples.push_back(sample);
    }
    return samples;
}