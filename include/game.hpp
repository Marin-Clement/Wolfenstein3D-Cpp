#pragma once

#include "Player.hpp"
#include "Map.hpp"
#include "Renderer.hpp"

class Game {
public:
    Game(int argc, char** argv);
    void run();

private:
    void setupGLUT(int argc, char** argv);
    void disableVSync();
    static void display();
    static void reshape(int width, int height);
    static void keyboard(unsigned char key, int x, int y);
    static void keyboardUp(unsigned char key, int x, int y);
    static void idle();

    static Player player;
    static Map map;
    static Renderer renderer;
    static int lastTime;
    static int frameCount;
    static int fps;
    static int lastFPSUpdateTime;
    static bool keys[256];
};