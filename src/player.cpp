// Player.cpp
#include "Player.hpp"
#include <cmath>

Player::Player() : x(6.0f), y(8.5f), angle(0.0f), movementSpeed(5.0f), rotationSpeed(20.0f) {}

void Player::update(bool* keys, const Map& map, float deltaTime) {
    float newX = x;
    float newY = y;

    if (keys['z']) { // Move forward
        newX += cos(angle) * movementSpeed * deltaTime;
        newY += sin(angle) * movementSpeed * deltaTime;
    }
    if (keys['s']) { // Move backward
        newX -= cos(angle) * movementSpeed * deltaTime;
        newY -= sin(angle) * movementSpeed * deltaTime;
    }

    // Check for collision with walls using buffer distance
    if (map.isWalkable(newX, y)) {
        x = newX;
    }
    if (map.isWalkable(x, newY)) {
        y = newY;
    }

    if (keys['q']) { // Turn left
        angle += 0.1f * rotationSpeed * deltaTime;
    }
    if (keys['d']) { // Turn right
        angle -= 0.1f * rotationSpeed * deltaTime;
    }
}

float Player::getX() const {
    return x;
}

float Player::getY() const {
    return y;
}

float Player::getAngle() const {
    return angle;
}