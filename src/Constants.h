#pragma once
#include "navigation/Point.h"
#include <filesystem>
#include <string>
#include <vector>

// TODO: Remove this and make translation file
const static std::string ABOUT_SECTION = R"(OpenATC version Alpha 0.1)";
const static std::filesystem::path CONFIG_PATH = "./config.toml";

// Simple waypoints
// TODO: Remove this and make parsing AIRAC
std::vector<Point> waypoints{
    {"SUDOB", {54.073055556, 28.389722222}},
    {"AGNEF", {54.485833333, 28.806111111}},
    {"LAPKI", {54.675000000, 29.481388889}},
    {"RATIN", {}},
    {"AMKUK", {}},
    {"BAPRO", {}},
    {"AGLAN", {}},
    {"GIDLI", {}},
    {"DIPOP", {}},
    {"LASRA", {}},
    {"AMDRO", {}},
    {"LONAF", {}},
    {"TAFAZ", {}},
};
