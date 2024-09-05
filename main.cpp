#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define screenWidth 640
#define screenHeight 480
#define mapWidth 24
#define mapHeight 24
#define fogDistance 15

std::vector<double> zBuffer(screenWidth);

struct Sprite {
    double x, y; // Position
    int texture; // Texture index
    double scale; // Scale factor
};

std::vector<Sprite> sprites = {
    {20.5, 11.5, 7, 0.5}, // Example sprite with scale 0.5
    {18.5, 4.5, 7, 0.5}   // Another example sprite with scale 0.5
};

int worldMap[mapWidth][mapHeight] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

double posX = 22, posY = 12;  // x and y start position
double dirX = -1, dirY = 0;  // initial direction vector
double planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane

std::unordered_map<int, bool> keyStates;

int frameCount = 0;
double lastTime = 0.0;
double fps = 0.0;

double deltaTime = 0.0;
double lastFrameTime = 0.0;


void calculateFPS() {
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - lastTime >= 1.0) {
        fps = double(frameCount) / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
        std::cout << "FPS: " << fps << std::endl;
    }
}

void renderFPS() {
    std::string fpsText = "FPS: " + std::to_string(fps);
    std::cout << fpsText << std::endl;
}

std::vector<GLuint> textures;

void removeColorFromImage(const std::string& inputFile, const std::string& outputFile, const unsigned char targetColor[3], float tolerance = 0.1f) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(inputFile.c_str(), &width, &height, &nrChannels, 4); // Load with RGBA channels
    if (!data) {
        std::cerr << "Failed to load image: " << inputFile << std::endl;
        return;
    }

    for (int i = 0; i < width * height; ++i) {
        unsigned char* pixel = data + i * 4; // Each pixel has 4 bytes (RGBA)
        unsigned char r = pixel[0];
        unsigned char g = pixel[1];
        unsigned char b = pixel[2];

        // Compute color distance
        float distance = sqrtf(powf(r - targetColor[0], 2) + powf(g - targetColor[1], 2) + powf(b - targetColor[2], 2));

        if (distance < tolerance * 255.0f) {
            // Set alpha to 0 (transparent)
            pixel[3] = 0;
        }
    }

    stbi_write_png(outputFile.c_str(), width, height, 4, data, width * 4);
    stbi_image_free(data);
}

void loadTextures() {
    std::vector<std::string> textureFiles = {
        "../assets/textures/105.bmp",
        "../assets/textures/47.bmp",
        "../assets/textures/2.bmp",
        "../assets/textures/3.bmp",
        "../assets/textures/4.bmp",
        "../assets/textures/5.bmp",
        "../assets/textures/98.bmp",
        "../assets/textures/soldier.png"
    };

    for (const auto& file : textureFiles) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Choose the format based on the number of channels
            if (nrChannels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else if (nrChannels == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);

            textures.push_back(texture);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << file << std::endl;
        }
    }
}

void drawSprites() {
    for (const auto& sprite : sprites) {
        double spriteX = sprite.x - posX;
        double spriteY = sprite.y - posY;

        double invDet = 1.0 / (planeX * dirY - dirX * planeY);

        double transformX = invDet * (dirY * spriteX - dirX * spriteY);
        double transformY = invDet * (-planeY * spriteX + planeX * spriteY);

        int spriteScreenX = int((screenWidth / 2) * (1 + transformX / transformY));

        int spriteHeight = abs(int(screenHeight / transformY * sprite.scale));
        int drawStartY = screenHeight / 2; // Draw sprite on the ground
        int drawEndY = drawStartY + spriteHeight;


        int spriteWidth = abs(int(screenHeight / transformY * sprite.scale));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0) drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;

        glBindTexture(GL_TEXTURE_2D, textures[sprite.texture]);

        glBegin(GL_QUADS);
        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * 64 / spriteWidth) / 256;
            if (transformY > 0 && stripe > 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {
                double shade = 1.0 - transformY / fogDistance;
                glColor3f(shade, shade, shade);

                glTexCoord2f(texX / 64.0, 0); glVertex2i(stripe, drawStartY);
                glTexCoord2f(texX / 64.0, 1); glVertex2i(stripe, drawEndY);
                glTexCoord2f((texX + 1) / 64.0, 1); glVertex2i(stripe + 1, drawEndY);
                glTexCoord2f((texX + 1) / 64.0, 0); glVertex2i(stripe + 1, drawStartY);
            }
        }
        glEnd();
    }
}

void drawFloor() {
    glBindTexture(GL_TEXTURE_2D, textures[6]); // Assuming texture 6 is for the floor

    for (int y = screenHeight / 2 + 1; y < screenHeight; y++) {
        double rayDirX0 = dirX - planeX;
        double rayDirY0 = dirY - planeY;
        double rayDirX1 = dirX + planeX;
        double rayDirY1 = dirY + planeY;

        int p = y - screenHeight / 2;
        double posZ = 0.5 * screenHeight;
        double rowDistance = posZ / p;

        double floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
        double floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

        double floorX = posX + rowDistance * rayDirX0;
        double floorY = posY + rowDistance * rayDirY0;

        glBegin(GL_QUADS);
        for (int x = 0; x < screenWidth; ++x) {
            int cellX = (int)(floorX);
            int cellY = (int)(floorY);

            if (cellX >= 0 && cellX < mapWidth && cellY >= 0 && cellY < mapHeight) {
                double tx = (floorX - cellX) * 64; // Assuming texture width is 64
                double ty = (floorY - cellY) * 64; // Assuming texture height is 64

                // Darken the floor the further away
                double shade = 1.0 - rowDistance / fogDistance;
                glColor3f(shade, shade, shade);

                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x, y);
                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x + 1, y);
                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x + 1, y + 1);
                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x, y + 1);
            }

            floorX += floorStepX;
            floorY += floorStepY;
        }
        glEnd();
    }
}

void drawCeiling() {
    glBindTexture(GL_TEXTURE_2D, textures[1]); // Assuming texture 1 is for the ceiling

    for (int y = 0; y < screenHeight / 2; y++) {
        double rayDirX0 = dirX - planeX;
        double rayDirY0 = dirY - planeY;
        double rayDirX1 = dirX + planeX;
        double rayDirY1 = dirY + planeY;

        int p = screenHeight / 2 - y;
        double posZ = 0.5 * screenHeight;
        double rowDistance = posZ / p;

        double ceilingStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
        double ceilingStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

        double ceilingX = posX + rowDistance * rayDirX0;
        double ceilingY = posY + rowDistance * rayDirY0;

        glBegin(GL_QUADS);
        for (int x = 0; x < screenWidth; ++x) {
            int cellX = (int)(ceilingX);
            int cellY = (int)(ceilingY);


            if (cellX >= 0 && cellX < mapWidth && cellY >= 0 && cellY < mapHeight) {
                double tx = (ceilingX - cellX) * 64; // Assuming texture width is 64
                double ty = (ceilingY - cellY) * 64; // Assuming texture height is 64

                // darker the further away
                double shade = 1.0 - rowDistance / fogDistance;
                glColor3f(shade, shade, shade);

                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x, y);
                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x + 1, y);
                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x + 1, y + 1);
                glTexCoord2f(tx / 64, ty / 64); glVertex2i(x, y + 1);
            }

            ceilingX += ceilingStepX;
            ceilingY += ceilingStepY;
        }
        glEnd();
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Initialize zBuffer
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<double>::infinity());

    // Draw the floor first
    drawFloor();

    // Draw the ceiling
    drawCeiling();

    // Draw the walls
    for (int x = 0; x < screenWidth; x++) {
        double cameraX = 2 * x / (double)screenWidth - 1;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = int(posX);
        int mapY = int(posY);

        double sideDistX;
        double sideDistY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);

        double perpWallDist;
        int stepX;
        int stepY;
        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
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

        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else           perpWallDist = (sideDistY - deltaDistY);

        int lineHeight = (int)(screenHeight / perpWallDist);

        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

        int texNum = worldMap[mapX][mapY] - 1;
        glBindTexture(GL_TEXTURE_2D, textures[texNum]);

        double wallX;
        if (side == 0) wallX = posY + perpWallDist * rayDirY;
        else           wallX = posX + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        // Calculate texture coordinates
        double texCoordX = wallX;
        double texCoordYStart = (drawStart - screenHeight / 2 + lineHeight / 2) / (double)lineHeight;
        double texCoordYEnd = (drawEnd - screenHeight / 2 + lineHeight / 2) / (double)lineHeight;

        // darker the further away
        double shade = 1.0 - perpWallDist / fogDistance;
        glColor3f(shade, shade, shade);

        // Draw the wall slice
        glBegin(GL_QUADS);
        glTexCoord2f(texCoordX, texCoordYStart); glVertex2i(x, drawStart);
        glTexCoord2f(texCoordX, texCoordYEnd); glVertex2i(x, drawEnd);
        glTexCoord2f(texCoordX + 1.0 / screenWidth, texCoordYEnd); glVertex2i(x + 1, drawEnd);
        glTexCoord2f(texCoordX + 1.0 / screenWidth, texCoordYStart); glVertex2i(x + 1, drawStart);
        glEnd();

        // Store the perpendicular wall distance in the zBuffer
        zBuffer[x] = perpWallDist;
    }

    // Draw the sprites
    drawSprites();

    glfwSwapBuffers(glfwGetCurrentContext());
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keyStates[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}

void processInput() {
    double moveSpeed = 5 * deltaTime; // Adjust speed by deltaTime
    double rotSpeed = 3.0 * deltaTime; // Adjust rotation speed by deltaTime

    if (keyStates[GLFW_KEY_W]) {
        if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == 0) posX += dirX * moveSpeed;
        if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == 0) posY += dirY * moveSpeed;
    }
    if (keyStates[GLFW_KEY_S]) {
        if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == 0) posX -= dirX * moveSpeed;
        if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == 0) posY -= dirY * moveSpeed;
    }
    if (keyStates[GLFW_KEY_A]) {
        double oldDirX = dirX;
        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }
    if (keyStates[GLFW_KEY_D]) {
        double oldDirX = dirX;
        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
}


int main() {
    unsigned char purple[3] = {152, 0, 136}; // Example purple color

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }



    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Raycaster", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);

    glEnable(GL_TEXTURE_2D);
    loadTextures();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //disable vsync
    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        double currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        processInput();
        renderScene();
        calculateFPS();

        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
