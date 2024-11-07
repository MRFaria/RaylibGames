#ifndef PLAYER_H
#define PLAYER_H

#include "globals.h"
#include "helpers.h"
#include "swept-aabb.h"  // Include the header where `helper::DynamicRectVsRect` is declared
#include "raylib.h"
#include "Level.h"

class Player {
public:
    Player();  // Constructor
    ~Player();  // Constructor
    void Update(Level& level);
    void Draw(Camera2D &camera);

    Vector2 GetPosition();
    Rectangle GetRect();
    void SetPosition(Vector2 pos);

private:
    Texture2D texture;
    float speed = 600.0;
    float acceleration = 2000.0;
    float deceleration = 1500.0;
    Vector2 velocity = {0.0, 0.0};
    Color color;      // Color of the player
    Rectangle playerRect;   // Rectangle for collision detection
    float rotation = 0.0;

    void ClampToWorldBounds();
    bool CheckCollisionsWithLevel(Level& level, Rectangle rectP, Vector2 velocityP, Vector2& contactPoint, Vector2& contactNormal, float& contactTime);
};

#endif // PLAYER_H
