//
// Created by marin on 05/09/2024.
//

#include "../../include/game/enemy.hpp"

#include <cmath>
#include <vector>
#include <GL/glew.h>

Enemy::Enemy(double x, double y, double z, int texture, double speed, double scale, double health) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->texture = texture;
    this->speed = speed;
    this->scale = scale;
    this->health = health;
}

void Enemy::update(double pos_x, double pos_y) {
   // use a simple AI to move towards the player but don't go through walls
    double dx = pos_x - x;
    double dy = pos_y - y;
    double distance = sqrt(dx * dx + dy * dy);

    // if player tp close move away
    if (distance < 1) {
        x -= dx / distance * speed;
        y -= dy / distance * speed;
        return;
    }

    // if player is close enough then stop moving
    if (distance < 3)
        return;

    // move towards the player
    x += dx / distance * speed;
    y += dy / distance * speed;
}

void Enemy::draw(const std::vector<double>& zBuffer, const std::vector<GLuint>& textures, double posX, double posY, int screenWidth, int screenHeight, double planeX, double planeY, double dirX, double dirY) const {
    double spriteX = x - posX;
    double spriteY = y - posY;

    double invDet = 1.0 / (planeX * dirY - dirX * planeY);

    double transformX = invDet * (dirY * spriteX - dirX * spriteY);
    double transformY = invDet * (-planeY * spriteX + planeX * spriteY);

    int spriteScreenX = int((screenWidth / 2) * (1 + transformX / transformY));

    // Calculate sprite height and width based on the screen height and sprite scale
    int spriteHeight = abs(int(screenHeight / transformY * scale));
    int drawStartY = -spriteHeight / 2 + screenHeight / 2 + z * spriteHeight;
    int drawEndY = drawStartY + spriteHeight;

    int spriteWidth = abs(int(screenHeight / transformY * scale));
    int drawStartX = -spriteWidth / 2 + spriteScreenX;
    if (drawStartX < 0) drawStartX = 0;
    int drawEndX = spriteWidth / 2 + spriteScreenX;

    // if the sprite is out of the screen or behind the player then skip
    if (drawStartX >= screenWidth || drawEndX <= 0 || transformY <= 0)
        return;

    glBindTexture(GL_TEXTURE_2D, textures[texture]);

    glBegin(GL_QUADS);
    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
        int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * 64 / spriteWidth) / 256;
        if (transformY > 0 && stripe > 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {
            double shade = 1.0 / (1.0 + transformY);
            glColor3f(shade, shade, shade);

            glTexCoord2f(texX / 64.0, 0); glVertex2i(stripe, drawStartY);
            glTexCoord2f(texX / 64.0, 1); glVertex2i(stripe, drawEndY);
            glTexCoord2f((texX + 1) / 64.0, 1); glVertex2i(stripe + 1, drawEndY);
            glTexCoord2f((texX + 1) / 64.0, 0); glVertex2i(stripe + 1, drawStartY);
        }
    }
    glEnd();
}