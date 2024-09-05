//
// Created by marin on 05/09/2024.
//


#pragma once
#include <vector>
#include <GL/glew.h>

class Enemy {
public:
    double x, y, z;
    int texture;
    double speed;
    double scale;
    double health;

    Enemy(double x, double y, double z, int texture, double speed, double scale, double health);
    void update(double pos_x, double pos_y);
    void draw(const std::vector<double>& zBuffer, const std::vector<GLuint>& textures, double posX, double posY, int screenWidth, int screenHeight, double planeX, double planeY, double dirX, double dirY) const;
};
