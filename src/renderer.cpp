// Renderer.cpp
#include "Renderer.hpp"
#include <GLUT/glut.h>
#include <sstream>
#include <iomanip>
#include "texture.hpp"
#include "raycast.hpp"

void Renderer::loadTextures(const std::string& directoryPath) {
    ::loadTextures(directoryPath);
}

void Renderer::display(const Player& player, const Map& map, int fps) {
    // Set background color to gray
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    castRays(player, map);

    // Disable depth testing before drawing text
    glDisable(GL_DEPTH_TEST);

    // Display FPS and player position
    glColor3f(1.0f, 1.0f, 1.0f);
    std::stringstream ss;
    ss << "FPS: " << fps;
    displayText(10, 20, ss.str());

    ss.str("");
    ss << "Position: (x=" << std::fixed << std::setprecision(2) << player.getX() << ", y=" << player.getY() << ")";
    displayText(10, 40, ss.str());

    // Re-enable depth testing after drawing text
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

void Renderer::updateFPS(int& frameCount, int& fps, int& lastFPSUpdateTime) {
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

void Renderer::displayText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}