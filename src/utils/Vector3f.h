#pragma once

struct Vector3f {
  float x;
  float y;
  float z;

  Vector3f operator+(const Vector3f& other) const {
    return {
      x + other.x,
      y + other.y,
      z + other.z
    };
  }
};
