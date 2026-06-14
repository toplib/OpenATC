#pragma once

#include "navigation/Position.h"
#include "utils/Vector3f.h"

namespace Physics {
  class PhysicsModel {
    public:
    virtual ~PhysicsModel() = default;

    virtual void tick(float dt) = 0;

    Position getPosition() {
      return m_position;
    }
    void setPosition(const Position &position) {
      m_position = m_position + position;
    }

    Vector3f getForce() {
      return m_force;
    }
    void applyForce(const Vector3f &force) {
      m_force = m_force + force;
    }

    private:
    Position m_position{0.0, 0.0, 0.0};
    Vector3f m_force{0.0f, 0.0f, 0.0f};
  };
}
