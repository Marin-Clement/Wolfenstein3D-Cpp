#include "raycast.hpp"

#include <GLUT/glut.h>
#include <cmath>
#include <texture.hpp>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

extern int worldMap[MAP_WIDTH][MAP_HEIGHT];
extern float playerX;
extern float playerY;
extern float playerAngle;

void castRays() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures["../assets/wall.png"]);

    for (int x = 0; x < w; x++) {
        float cameraX = 2 * x / float(w) - 1;
        float rayDirX = cos(playerAngle) + cameraX * sin(playerAngle);
        float rayDirY = sin(playerAngle) - cameraX * cos(playerAngle);

        int mapX = int(playerX);
        int mapY = int(playerY);

        float sideDistX;
        float sideDistY;

        float deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
        float perpWallDist;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (playerX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (playerY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
        }

        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        if (side == 0) perpWallDist = (mapX - playerX + (1 - stepX) / 2) / rayDirX;
        else perpWallDist = (mapY - playerY + (1 - stepY) / 2) / rayDirY;

        int lineHeight = (int)(h / perpWallDist);

        int drawStart = -lineHeight / 2 + h / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + h / 2;
        if (drawEnd >= h) drawEnd = h - 1;

        float wallX;
        if (side == 0) wallX = playerY + perpWallDist * rayDirY;
        else wallX = playerX + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        float texCoordX = wallX;
        float texCoordYStart = 0.0f;
        float texCoordYEnd = 1.0f;

        if (drawStart == 0) {
            texCoordYStart = (lineHeight / 2.0f - h / 2.0f) / lineHeight;
        }
        if (drawEnd == h - 1) {
            texCoordYEnd = (lineHeight / 2.0f + h / 2.0f) / lineHeight;
        }

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);

        glTexCoord2f(texCoordX, texCoordYStart);
        glVertex2i(x, drawStart);

        glTexCoord2f(texCoordX, texCoordYEnd);
        glVertex2i(x, drawEnd);

        glTexCoord2f(texCoordX + (1.0f / w), texCoordYEnd);
        glVertex2i(x + 1, drawEnd);

        glTexCoord2f(texCoordX + (1.0f / w), texCoordYStart);
        glVertex2i(x + 1, drawStart);

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}