#ifndef PLAYER_H
#define PLAYER_H

#include "globals.h"
#include "helpers.h"
#include "swept-aabb.h"  // Include the header where `helper::DynamicRectVsRect` is declared
#include "raylib.h"
#include "level.h"

class Player {
public:
    float speed = 600.0;
    float acceleration = 3000.0;
    float deceleration = 2000.0;
    Vector2 velocity = {0.0,0.0};
    Color color;      // Color of the player
    Rectangle rect;   // Rectangle for collision detection

    Player() {
        // Initialize player properties
        SetPosition(Vector2{400, 300}); // Center of the screen
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

    void Update(Level& level) 
    {

        Vector2 originalPosition = GetPosition(); // Store original position

        // Movement logic
        Vector2 direction = {0, 0}; // Initialize movement vector
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            direction.x = 1;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            direction.x = -1;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            direction.y = -1;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            direction.y = 1;

    Vector2 dv = direction * acceleration * GetFrameTime();
    // Apply deceleration if no direction is pressed
    if (direction.x == 0) {
        if (std::abs(velocity.x) < deceleration * GetFrameTime()) {
            velocity.x = 0; // Stop if deceleration would push velocity below zero
        } else {
            velocity.x -= (velocity.x > 0 ? deceleration : -deceleration) * GetFrameTime();
        }
    }

    if (direction.y == 0) {
        if (std::abs(velocity.y) < deceleration * GetFrameTime()) {
            velocity.y = 0; // Stop if deceleration would push velocity below zero
        } else {
            velocity.y -= (velocity.y > 0 ? deceleration : -deceleration) * GetFrameTime();
        }
    }
        if (direction.y == 0  && std::abs(velocity.y) > 0)
        {
            if (velocity.y < 0)
                dv.y = deceleration*GetFrameTime();
            else if (velocity.y > 0)
                dv.y = -deceleration*GetFrameTime();
        }

        velocity=velocity+dv;

        float currentSpeed = Vector2Length(velocity);
        if (currentSpeed > speed) {
            velocity = Vector2Scale(Vector2Normalize(velocity), speed);
        }
        Vector2 contactPoint, contactNormal;
        float contactTime;

        // Use the player's original collision rect size
        Rectangle playerColRect = rect;
        playerColRect.y -= TILE_WALL_OUTLINE_WIDTH/2.0;
        playerColRect.x -= TILE_WALL_OUTLINE_WIDTH/2.0;
        playerColRect.width += TILE_WALL_OUTLINE_WIDTH;
        playerColRect.height += TILE_WALL_OUTLINE_WIDTH;
        // Handle X-axis movement
        Vector2 xVelocity = {velocity.x, 0}; // Only move along the X axis
        if (CheckCollisionsWithLevel(level, playerColRect, xVelocity, contactPoint, contactNormal, contactTime)) {
            // Stop X movement if collision occurs
            xVelocity = xVelocity + (contactNormal * Vector2(std::abs(velocity.x) + 1, std::abs(velocity.y)) * (1 - contactTime));
        }
        SetPosition(GetPosition() + xVelocity * GetFrameTime());

        // Handle Y-axis movement
        Vector2 yVelocity = {0, velocity.y}; // Only move along the Y axis
        if (CheckCollisionsWithLevel(level, playerColRect, yVelocity, contactPoint, contactNormal, contactTime)) {
            // Stop Y movement if collision occurs
            yVelocity = yVelocity + (contactNormal * Vector2(std::abs(velocity.x), std::abs(velocity.y) + 1) * (1 - contactTime));
        }
        SetPosition(GetPosition() + yVelocity * GetFrameTime());

        // Optional: Clamp the player's position to the world boundaries if needed
        ClampToWorldBounds();
    }

    void Draw() 
    {
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

    bool CheckCollisionsWithLevel(Level& level, Rectangle rectP, Vector2 velocityP, Vector2& contactPoint, Vector2& contactNormal, float& contactTime) {
        // Check all tiles in the level for collision
        helper::DynamicRect dynamicRect;
        dynamicRect.rect = rectP;
        dynamicRect.vel = velocityP; 
        for (int x = 0; x < level.GetWidth(); x++) {
            for (int y = 0; y < level.GetHeight(); y++) {
                if (level.GetTile(x, y) == TILE_WALL) {
                    Rectangle staticRect = { x * (float)N_TILE_WIDTH, y * (float)N_TILE_HEIGHT, (float)N_TILE_WIDTH, (float)N_TILE_HEIGHT };
                    if (helper::DynamicRectVsRect(dynamicRect, GetFrameTime(), staticRect, contactPoint, contactNormal, contactTime)) {
                        return true; // Collision detected
                    }
                }
            }
        }

        return false; // No collision detected
    }
};

#endif