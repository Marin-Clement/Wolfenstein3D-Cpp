//
// Created by Marin Clément on 29/08/2024.
//

#pragma once

class Player {
  public:
    Player();
    ~Player();
    void move(float deltax, float deltay);
    void update();
    void draw();
private:
    float x, y;
    float speed;
};

