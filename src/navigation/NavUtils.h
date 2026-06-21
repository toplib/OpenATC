#pragma once

#include <cmath>
#include <numbers>

#include "Coordinates.h"

static constexpr double EARTH_M = 111319.9;

inline double toRad(double deg) {
  return deg * std::numbers::pi / 180.0;
}

inline double toDeg(double rad) {
  return rad * 180.0 / std::numbers::pi;
}

inline double normalizeHeading(double hdg) {
  hdg = std::fmod(hdg, 360.0);
  if (hdg < 0) hdg += 360.0;
  return hdg;
}

inline double equirectDistance(const Coordinates &a, const Coordinates &b) {
  double dlat = (b.latitude - a.latitude) * EARTH_M;
  double dlon = (b.longitude - a.longitude) * EARTH_M
              * std::cos(toRad(a.latitude));
  return std::sqrt(dlat * dlat + dlon * dlon);
}

inline double bearing(const Coordinates &from, const Coordinates &to) {
  double φ1 = toRad(from.latitude);
  double φ2 = toRad(to.latitude);
  double Δλ = toRad(to.longitude - from.longitude);

  double θ = std::atan2(
    std::sin(Δλ) * std::cos(φ2),
    std::cos(φ1) * std::sin(φ2) - std::sin(φ1) * std::cos(φ2) * std::cos(Δλ)
  );

  return normalizeHeading(toDeg(θ));
}

inline double headingError(double current, double target) {
  double error = target - current;
  return std::fmod(error + 540.0, 360.0) - 180.0;
}

inline double turnToward(double current, double target, double rate, double dt) {
  double err = headingError(current, target);
  if (std::abs(err) < 1.0) return normalizeHeading(target);
  return normalizeHeading(current + std::copysign(rate * dt, err));
}
