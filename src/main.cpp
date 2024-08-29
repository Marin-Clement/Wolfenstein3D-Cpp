#define GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#include <OpenGL/OpenGL.h> // Include for CGLContextObj and related functions
#include <OpenGL/gl.h>    // Header File For The OpenGL32 Library
#include <OpenGL/glu.h>   // Header File For The GLu32 Library
#include <GLUT/glut.h>    // Header File For The GLut Library
#include <cmath>          // Header File For Math functions
#include <iostream>
#include <sstream>
#include <iomanip>
#include <texture.hpp>

#include "raycast.hpp"

#define kWindowWidth    1280
#define kWindowHeight   720

int lastTime;
int frameCount = 0;
int fps = 0;
int lastFPSUpdateTime = 0;

const int mapWidth = 24;
const int mapHeight = 24;
int worldMap[mapWidth][mapHeight] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

float playerX = 6.0f;
float playerY = 8.5f;
float playerAngle = 0.0f;
float movementSpeed = 5.0f;
float rotationSpeed = 20.0f;

bool keys[256]; // Array to keep track of which keys are pressed

const float bufferDistance = 0.5f; // Define a small buffer distance

void update(float deltaTime) {
    float newX = playerX;
    float newY = playerY;

    if (keys['z']) { // Move forward
        newX += cos(playerAngle) * movementSpeed * deltaTime;
        newY += sin(playerAngle) * movementSpeed * deltaTime;
    }
    if (keys['s']) { // Move backward
        newX -= cos(playerAngle) * movementSpeed * deltaTime;
        newY -= sin(playerAngle) * movementSpeed * deltaTime;
    }

    // Check for collision with walls using buffer distance
    if (worldMap[int(newX + cos(playerAngle) * bufferDistance)][int(playerY)] == 0) {
        playerX = newX;
    }
    if (worldMap[int(playerX)][int(newY + sin(playerAngle) * bufferDistance)] == 0) {
        playerY = newY;
    }

    if (keys['q']) { // Turn left
        playerAngle += 0.1f * rotationSpeed * deltaTime;
    }
    if (keys['d']) { // Turn right
        playerAngle -= 0.1f * rotationSpeed * deltaTime;
    }
}

void displayText(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void updateFPS() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    frameCount++;
    if (currentTime - lastFPSUpdateTime > 1000) {
        fps = frameCount;
        frameCount = 0;
        lastFPSUpdateTime = currentTime;
        std::ostringstream oss;
        oss << "Wolfenstein 3D Clone - FPS: " << fps;
        glutSetWindowTitle(oss.str().c_str());
    }
}

void display() {
    // Set background color to gray
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    castRays();

    // Disable depth testing before drawing text
    glDisable(GL_DEPTH_TEST);

    // Display FPS and player position
    glColor3f(1.0f, 1.0f, 1.0f);
    std::stringstream ss;
    ss << "FPS: " << fps;
    displayText(10, 20, ss.str());

    ss.str("");
    ss << "Position: (x=" << std::fixed << std::setprecision(2) << playerX << ", y=" << playerY << ")";
    displayText(10, 40, ss.str());

    // Re-enable depth testing after drawing text
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, height, 0.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void idle() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    update(deltaTime);
    updateFPS();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("Wolfenstein 3D Clone");

    glEnable(GL_DEPTH_TEST);

    std::vector<std::string> textureFiles = {"../assets/wall.png", "../assets/wall1.png"};
    loadTextures(textureFiles);

    lastTime = glutGet(GLUT_ELAPSED_TIME);

    // Disable V-Sync
#ifdef __APPLE__
    GLint sync = 0;
    CGLContextObj ctx = CGLGetCurrentContext();
    CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
#else
    // For other platforms, use appropriate method to disable V-Sync
#endif

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}