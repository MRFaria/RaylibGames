#ifndef PLAYER_H
#define PLAYER_H

#include "globals.h"
#include "operators.h"
#include "swept-aabb.h"  // Include the header where `helper::DynamicRectVsRect` is declared
#include "raylib.h"
#include "level.h"

class Player {
public:
    Vector2 velocity; // Player's movement speed
    Color color;      // Color of the player
    Rectangle rect;   // Rectangle for collision detection

    Player() {
        // Initialize player properties
        SetPosition(Vector2{400, 300}); // Center of the screen
        velocity.x = 400.0f;            // Movement speed
        velocity.y = 400.0f;            // Movement speed
        color = BLUE;                   // Player color
        rect.height = 50;
        rect.width = rect.height;
    }

    Vector2 GetPosition()
    {
        return Vector2(rect.x, rect.y);
    }

    void SetPosition(Vector2 pos)
    {
        rect.x = pos.x;
        rect.y = pos.y;
    }

void Update(Level& level) {
    Vector2 originalPosition = GetPosition(); // Store original position
    
    // Movement logic
    Vector2 movement = {0, 0}; // Initialize movement vector
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        movement.x += velocity.x * GetFrameTime();
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        movement.x -= velocity.x * GetFrameTime();
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        movement.y -= velocity.y * GetFrameTime();
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        movement.y += velocity.y * GetFrameTime();

    // Update position based on movement
    rect.x += movement.x;
    rect.y += movement.y;

    CheckCollisionsWithLevel
    // Clamp player's position to the world boundaries
    ClampToWorldBounds();
}

    void Draw() {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
    }

private:
    void ClampToWorldBounds() {
        if (rect.x < 0) rect.x = 0;  // Left boundary
        if (rect.x + rect.width > GetScreenWidth() * N_TILE_WIDTH) 
            rect.x = GetScreenWidth() * N_TILE_WIDTH - rect.width;  // Right boundary

        if (rect.y < 0) rect.y = 0;  // Top boundary
        if (rect.y + rect.height > GetScreenHeight() * N_TILE_HEIGHT) 
            rect.y = GetScreenHeight() * N_TILE_HEIGHT - rect.height;  // Bottom boundary
    }

    bool CheckCollisionsWithLevel(Level& level, Rectangle rectP, Vector2 velocityP, & contactPoint, Vector2& contactNormal, float& contactTime) {
        // Check all tiles in the level for collision
        helper::DynamicRect dynamicRect;
        dynamicRect.rect = rectP;
        dynamicRect.vel = velocityP; 
        for (int x = 0; x < level.GetWidth(); x++) {
            for (int y = 0; y < level.GetHeight(); y++) {
                if (level.GetTile(x, y) == TILE_WALL) {
                    Rectangle staticRect = { x * (float)N_TILE_WIDTH, y * (float)N_TILE_HEIGHT, (float)N_TILE_WIDTH, (float)N_TILE_HEIGHT };
                    if (helper::DynamicRectVsRect(dynamicRect, timeStep, staticRect, contactPoint, contactNormal, contactTime)) {
                        return true; // Collision detected
                    }
                }
            }
        }

        return false; // No collision detected
    }
};

#endif