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

struct Ray {
    float dirX, dirY;
    int mapX, mapY;
    float sideDistX, sideDistY;
    float deltaDistX, deltaDistY;
    int stepX, stepY;
    int side;
};

struct Wall {
    int drawStart, drawEnd;
    float texCoordX, texCoordYStart, texCoordYEnd;
    int lineHeight;
};

void initializeOpenGL() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures["../assets/wall.png"]);
}

void calculateRayDirection(Ray &ray, float cameraX) {
    ray.dirX = cos(playerAngle) + cameraX * sin(playerAngle);
    ray.dirY = sin(playerAngle) - cameraX * cos(playerAngle);
}

void performDDA(Ray &ray) {
    int hit = 0;

    while (hit == 0) {
        if (ray.sideDistX < ray.sideDistY) {
            ray.sideDistX += ray.deltaDistX;
            ray.mapX += ray.stepX;
            ray.side = 0;
        } else {
            ray.sideDistY += ray.deltaDistY;
            ray.mapY += ray.stepY;
            ray.side = 1;
        }
        if (worldMap[ray.mapX][ray.mapY] > 0) hit = 1;
    }
}

void drawWall(int x, const Wall &wall, int w) {
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);

    glTexCoord2f(wall.texCoordX, wall.texCoordYStart);
    glVertex2i(x, wall.drawStart);

    glTexCoord2f(wall.texCoordX, wall.texCoordYEnd);
    glVertex2i(x, wall.drawEnd);

    glTexCoord2f(wall.texCoordX + (1.0f / w), wall.texCoordYEnd);
    glVertex2i(x + 1, wall.drawEnd);

    glTexCoord2f(wall.texCoordX + (1.0f / w), wall.texCoordYStart);
    glVertex2i(x + 1, wall.drawStart);

    glEnd();
}

void castRays() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    initializeOpenGL();

    for (int x = 0; x < w; x++) {
        float cameraX = 2 * x / float(w) - 1;
        Ray ray;
        calculateRayDirection(ray, cameraX);

        ray.mapX = int(playerX);
        ray.mapY = int(playerY);

        ray.deltaDistX = (ray.dirX == 0) ? 1e30 : fabs(1 / ray.dirX);
        ray.deltaDistY = (ray.dirY == 0) ? 1e30 : fabs(1 / ray.dirY);

        if (ray.dirX < 0) {
            ray.stepX = -1;
            ray.sideDistX = (playerX - ray.mapX) * ray.deltaDistX;
        } else {
            ray.stepX = 1;
            ray.sideDistX = (ray.mapX + 1.0 - playerX) * ray.deltaDistX;
        }
        if (ray.dirY < 0) {
            ray.stepY = -1;
            ray.sideDistY = (playerY - ray.mapY) * ray.deltaDistY;
        } else {
            ray.stepY = 1;
            ray.sideDistY = (ray.mapY + 1.0 - playerY) * ray.deltaDistY;
        }

        performDDA(ray);

        float perpWallDist;
        if (ray.side == 0) perpWallDist = (ray.mapX - playerX + (1 - ray.stepX) / 2) / ray.dirX;
        else perpWallDist = (ray.mapY - playerY + (1 - ray.stepY) / 2) / ray.dirY;

        Wall wall;
        wall.lineHeight = (int)(h / perpWallDist);

        wall.drawStart = -wall.lineHeight / 2 + h / 2;
        if (wall.drawStart < 0) wall.drawStart = 0;
        wall.drawEnd = wall.lineHeight / 2 + h / 2;
        if (wall.drawEnd >= h) wall.drawEnd = h - 1;

        float wallX;
        if (ray.side == 0) wallX = playerY + perpWallDist * ray.dirY;
        else wallX = playerX + perpWallDist * ray.dirX;
        wallX -= floor(wallX);

        wall.texCoordX = wallX;
        wall.texCoordYStart = 0.0f;
        wall.texCoordYEnd = 1.0f;

        if (wall.drawStart == 0) {
            wall.texCoordYStart = (wall.lineHeight / 2.0f - h / 2.0f) / wall.lineHeight;
        }
        if (wall.drawEnd == h - 1) {
            wall.texCoordYEnd = (wall.lineHeight / 2.0f + h / 2.0f) / wall.lineHeight;
        }

        drawWall(x, wall, w);
    }

    glDisable(GL_TEXTURE_2D);
}