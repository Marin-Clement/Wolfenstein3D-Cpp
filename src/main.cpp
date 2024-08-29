// Preprocessor Directives
#define GL_SILENCE_DEPRECATION

// Include Statements
#include <OpenGL/OpenGL.h>  // For CGLContextObj and related functions
#include <OpenGL/gl.h>      // OpenGL functions
#include <OpenGL/glu.h>     // GLU functions
#include <GLUT/glut.h>      // GLUT functions
#include <cmath>            // Math functions
#include <iostream>         // Standard I/O streams
#include <sstream>          // String streams
#include <iomanip>          // Input/Output manipulation
#include <texture.hpp>      // Texture loading
#include "raycast.hpp"      // Raycasting functionality

// Constants
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr int mapWidth = 24;
constexpr int mapHeight = 24;
constexpr float bufferDistance = 0.5f;

// Globals
int lastTime;
int frameCount = 0;
int fps = 0;
int lastFPSUpdateTime = 0;

float playerX = 6.0f;
float playerY = 8.5f;
float playerAngle = 0.0f;
float movementSpeed = 5.0f;
float rotationSpeed = 20.0f;

bool keys[256] = {false}; // Key state array

// Map Data
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

// Function Declarations
void update(float deltaTime);
void displayText(float x, float y, const std::string &text);
void updateFPS();
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void idle();
void setupGLUT(int argc, char** argv);
void disableVSync();

int main(int argc, char** argv) {
    setupGLUT(argc, argv);
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    disableVSync();
    glutMainLoop();
    return 0;
}

// Function Definitions

void setupGLUT(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("Wolfenstein 3D Clone");

    glEnable(GL_DEPTH_TEST);

    std::vector<std::string> textureFiles = {"../assets/wall.png", "../assets/wall1.png"};
    loadTextures(textureFiles);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIdleFunc(idle);
}

void disableVSync() {
#ifdef __APPLE__
    GLint sync = 0;
    CGLContextObj ctx = CGLGetCurrentContext();
    CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
#else
    // Implement V-Sync disabling for other platforms here
#endif
}

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
