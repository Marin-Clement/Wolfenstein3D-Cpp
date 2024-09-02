// Map.hpp

#pragma once

class Map {
public:
    Map();
    bool isWalkable(int x, int y) const;
    static const int width = 24;
    static const int height = 24;
    const int (*getWorldMap() const)[width];

private:
    int worldMap[width][height];
};