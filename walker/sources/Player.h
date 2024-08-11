#ifndef PLAYER_H
#define PLAYER_H

#include "Animation.h"

class Player {
public:
    Player(float, float);
    ~Player();
    void Update();
    void Draw();
private:
    Vector2 _position;
    Vector2 _velocity;
    float _screenWidth;
    float _screenHeight;
    const float _scale = 0.5f; // Scale factor for the character
    const float _speed = 400.0f; // Character movement speed in pixels per second
    bool flip = false;
    Animation _charIdleAnimation;
};

#endif
