// Player.hpp

#pragma once

#include "Map.hpp"

class Player {
public:
    Player();
    void update(bool* keys, const Map& map, float deltaTime);
    float getX() const;
    float getY() const;
    float getAngle() const;

private:
    float x;
    float y;
    float angle;
    float movementSpeed;
    float rotationSpeed;
};