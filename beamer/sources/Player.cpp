#include "Player.h"

Player::Player() : Entity{"textures/circle.png"}
{

}

Player::~Player()
{

}

void Player::Update(Level& level)
{
    // figure out keyboard mapping later.
    Vector2 direction = {0, 0}; // Initialize movement vector

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        direction.x = 1;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        direction.x = -1;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        direction.y = -1;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        direction.y = 1;

    VelocityMove(direction);
}