#pragma once
#include "Coordinates.h"

namespace Simulation {
    class Entity {
    public:
        virtual ~Entity() = default;

        virtual void setCoordinates(Coordinates coordinates);
        virtual Coordinates getCoordinates();
    private:
        Coordinates m_coordinates = {0.0, 0.0};
    };
}
