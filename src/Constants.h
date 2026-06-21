#pragma once

#include "navigation/Coordinates.h"
#include "navigation/Point.h"
#include <filesystem>
#include <string>
#include <vector>

const static std::string ABOUT_SECTION = R"(OpenATC version Alpha 0.1)";
const static std::filesystem::path CONFIG_PATH = "./config.toml";

inline std::vector<Point> waypoints{
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

namespace Airports {

struct TaxiwayData {
  std::string name;
  std::vector<Coordinates> points;
};

struct RunwayData {
  std::string name;
  Coordinates threshold;
  Coordinates holdShort;
  double heading;
  double length;
};

struct GateData {
  std::string name;
  Coordinates position;
};

struct AirportModel {
  std::string icao;
  std::vector<TaxiwayData> taxiways;
  std::vector<RunwayData> runways;
  std::vector<GateData> gates;
};

inline const AirportModel UMMS{
    "UMMS",
    {
        {"A",
         {
             {53.8820, 28.0250},
             {53.8825, 28.0280},
             {53.8840, 28.0300},
             {53.8860, 28.0305},
             {53.8885, 28.0310},
         }},
        {"B",
         {
             {53.8860, 28.0305},
             {53.8860, 28.0350},
             {53.8860, 28.0400},
             {53.8840, 28.0400},
         }},
        {"D",
         {
             {53.8885, 28.0310},
             {53.8890, 28.0305},
             {53.8900, 28.0300},
         }},
    },
    {
        {"31R", {53.8900, 28.0300}, {53.8885, 28.0310}, 310.0, 2500},
    },
    {
        {"G1", {53.8820, 28.0250}},
        {"G2", {53.8840, 28.0400}},
    },
};

} // namespace Airports
