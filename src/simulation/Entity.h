#pragma once
#include "Coordinates.h"

namespace Simulation {
    class Entity {
    public:
        virtual ~Entity() = default;

    private:
        Coordinates m_coordinates = {0.0, 0.0};
    };
}
