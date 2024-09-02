// Game.cpp
#include <OpenGL/OpenGL.h>  // For CGLContextObj and related functions
#include <OpenGL/gl.h>      // OpenGL functions
#include <OpenGL/glu.h>     // GLU functions
#include <GLUT/glut.h>      // GLUT functions

#include "Game.hpp"


Player Game::player;
Map Game::map;
Renderer Game::renderer;
int Game::lastTime;
int Game::frameCount = 0;
int Game::fps = 0;
int Game::lastFPSUpdateTime = 0;
bool Game::keys[256] = {false};

Game::Game(int argc, char** argv) {
    setupGLUT(argc, argv);
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    disableVSync();
}

void Game::run() {
    glutMainLoop();
}

void Game::setupGLUT(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Wolfenstein 3D Clone");

    glEnable(GL_DEPTH_TEST);

    renderer.loadTextures("../assets/textures");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIdleFunc(idle);
}

void Game::disableVSync() {
#ifdef __APPLE__
    GLint sync = 0;
    CGLContextObj ctx = CGLGetCurrentContext();
    CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
#else
    glutSetOption(GLUT_SWAP_CONTROL, 0);
#endif
}

void Game::display() {
    renderer.display(player, map, fps);
}

void Game::reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, height, 0.0);
    glMatrixMode(GL_MODELVIEW);
}

void Game::keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
}

void Game::keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void Game::idle() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    player.update(keys, map, deltaTime);
    renderer.updateFPS(frameCount, fps, lastFPSUpdateTime);
    glutPostRedisplay();
}