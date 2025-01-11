#include "Player.h"

Player::Player(Level &level) : Entity{"textures/circle.png", level}
{


}

Player::~Player()
{

}

void Player::Update(std::vector<Entity> &entities)
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

    VelocityMove(direction, entities);
}

void Player::Draw(Camera2D camera)
{

    BeginMode2D(camera);
    Vector2 rayToMouse = GetScreenToWorld2D(GetMousePosition(), camera) - GetPosition();
    DrawLineV(GetPosition(), GetScreenToWorld2D(GetMousePosition(), camera), YELLOW);
    float angle = Vector2Angle(Vector2Normalize(rayToMouse), Vector2(1,0)) - PI/2;
    _rotation = -(angle*180/PI);
    //DrawText((std::string("angle: ") + std::to_string(angle)).c_str(), GetPosition().x, GetPosition().y+100, 15, YELLOW);
    EndMode2D;
    ((Entity*)this)->Draw(camera);
}
