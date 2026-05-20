#pragma once
#include "simulation/Entity.h"

namespace Simulation {
    class AirCraft : public Entity {
    public:
        AirCraft();
        ~AirCraft() override;

    private:
        double m_speed; // knots

    };
}
