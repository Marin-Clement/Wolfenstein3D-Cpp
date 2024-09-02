// raycast.cpp
#include "raycast.hpp"
#include <GLUT/glut.h>
#include <cmath>
#include "texture.hpp"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

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
    int textureIndex;
};

void initializeOpenGL() {
    glEnable(GL_TEXTURE_2D);
}

void calculateRayDirection(Ray &ray, float cameraX, float playerAngle) {
    ray.dirX = cos(playerAngle) + cameraX * sin(playerAngle);
    ray.dirY = sin(playerAngle) - cameraX * cos(playerAngle);
}

void performDDA(Ray &ray, Wall &wall, const int worldMap[MAP_WIDTH][MAP_HEIGHT]) {
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
        if (worldMap[ray.mapX][ray.mapY] > 0) {
            hit = 1;
            wall.textureIndex = worldMap[ray.mapX][ray.mapY];
        }
    }
}

void drawWall(int x, const Wall &wall, int w) {
    glBindTexture(GL_TEXTURE_2D, textures[std::to_string(wall.textureIndex)]);

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

void castRays(const Player& player, const Map& map) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    initializeOpenGL();

    const int (*worldMap)[MAP_WIDTH] = map.getWorldMap();

    for (int x = 0; x < w; x++) {
        float cameraX = 2 * x / float(w) - 1;
        Ray ray;
        calculateRayDirection(ray, cameraX, player.getAngle());

        ray.mapX = int(player.getX());
        ray.mapY = int(player.getY());

        ray.deltaDistX = (ray.dirX == 0) ? 1e30 : fabs(1 / ray.dirX);
        ray.deltaDistY = (ray.dirY == 0) ? 1e30 : fabs(1 / ray.dirY);

        if (ray.dirX < 0) {
            ray.stepX = -1;
            ray.sideDistX = (player.getX() - ray.mapX) * ray.deltaDistX;
        } else {
            ray.stepX = 1;
            ray.sideDistX = (ray.mapX + 1.0 - player.getX()) * ray.deltaDistX;
        }
        if (ray.dirY < 0) {
            ray.stepY = -1;
            ray.sideDistY = (player.getY() - ray.mapY) * ray.deltaDistY;
        } else {
            ray.stepY = 1;
            ray.sideDistY = (ray.mapY + 1.0 - player.getY()) * ray.deltaDistY;
        }

        Wall wall;
        performDDA(ray, wall, worldMap);

        float perpWallDist;
        if (ray.side == 0) perpWallDist = (ray.mapX - player.getX() + (1 - ray.stepX) / 2) / ray.dirX;
        else perpWallDist = (ray.mapY - player.getY() + (1 - ray.stepY) / 2) / ray.dirY;

        wall.lineHeight = (int)(h / perpWallDist);

        wall.drawStart = -wall.lineHeight / 2 + h / 2;
        if (wall.drawStart < 0) wall.drawStart = 0;
        wall.drawEnd = wall.lineHeight / 2 + h / 2;
        if (wall.drawEnd >= h) wall.drawEnd = h - 1;

        float wallX;
        if (ray.side == 0) wallX = player.getY() + perpWallDist * ray.dirY;
        else wallX = player.getX() + perpWallDist * ray.dirX;
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