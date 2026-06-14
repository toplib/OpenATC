#pragma once

struct Position {
  double x;
  double y;
  double z;

  Position operator+(const Position& other) const {
    return {
      x + other.x,
      y + other.y,
      z + other.z
    };
  }

  Position operator-(const Position& other) const {
    return {
      x - other.x,
      y - other.y,
      z - other.z
    };
  }
};
