#pragma once
#include "navigation/Coordinates.h"
#include "physics/PhysicsModel.h"
#include "simulation/aircraft/AircraftModel.h"

namespace Simulation {
class Aircraft : public Physics::PhysicsModel {
public:
  Aircraft();
  ~Aircraft() override;

  void tick(float dt) override;

private:
  double m_frequency;
  Coordinates m_position;
  double m_speed; // knots
  
  AircraftModel m_model;
};
} // namespace Simulation
