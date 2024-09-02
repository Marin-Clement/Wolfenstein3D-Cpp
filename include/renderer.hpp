// Renderer.hpp

#pragma once

#include "Player.hpp"
#include "Map.hpp"
#include <string>


class Renderer {
public:
    void loadTextures(const std::string& directoryPath);
    void display(const Player& player, const Map& map, int fps);
    void updateFPS(int& frameCount, int& fps, int& lastFPSUpdateTime);

private:
    void displayText(float x, float y, const std::string& text);
};
