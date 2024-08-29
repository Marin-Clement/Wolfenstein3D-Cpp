//
// Created by Marin Clément on 29/08/2024.
//

#include "player.hpp"
#include <OpenGL/gl.h>    // Header File For The OpenGL32 Library

Player::Player() {
    x = 0;
    y = 0;
    speed = 0.1;
}

Player::~Player() {
}

void Player::move(float deltax, float deltay) {
    x += deltax;
    y += deltay;
}

void Player::update() {
}

void Player::draw() {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.1f);
    glVertex2f( 0.1f, -0.1f);
    glVertex2f( 0.1f,  0.1f);
    glVertex2f(-0.1f,  0.1f);
    glEnd();
    glPopMatrix();
}