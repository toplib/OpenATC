#pragma once

#include <random>
#include <string>
#include <vector>

namespace Sim {

struct Wind {
  double direction;
  double speed;
  double gust;
  double directionVariation;
};

struct Visibility {
  double horizontal;
  double vertical;
};

struct CloudLayer {
  enum Type { CLEAR, FEW, SCT, BKN, OVC };
  Type type;
  double base;
};

struct Environment {
  Wind wind;
  Visibility visibility;
  std::vector<CloudLayer> clouds;
  double qnh;
  double temperature;
  double dewPoint;
  double timeOfDay;

  char atisLetter = 'A';

  void tick(double dt) {
    if (wind.gust > wind.speed) {
      static std::mt19937 rng{std::random_device{}()};
      std::uniform_real_distribution<double> gustDist(wind.speed, wind.gust);
      wind.speed = gustDist(rng);
    }
  }

  std::string getAtisMessage(const std::string &airport) const {
    std::string msg;
    msg += airport + " ATIS information " + atisLetter + " ";
    msg += std::to_string((int)wind.direction) + " at " + std::to_string((int)wind.speed) + " knots";
    if (wind.gust > 0) msg += " gusting " + std::to_string((int)wind.gust);
    msg += ", visibility " + std::to_string((int)(visibility.horizontal / 1000)) + " kilometers";
    msg += ", QNH " + std::to_string((int)qnh);
    msg += ", temperature " + std::to_string((int)temperature);
    return msg;
  }
};

} // namespace Sim
