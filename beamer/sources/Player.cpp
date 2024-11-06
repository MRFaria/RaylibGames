#include "Player.h"

// Constructor
Player::Player() {
    SetPosition(Vector2{200, 200}); // Center of the screen
    color = BLUE;                   // Player color
    rect.height = 50;
    rect.width = rect.height;
}

Vector2 Player::GetPosition() {
    return Vector2{rect.x, rect.y};
}

Rectangle Player::GetRect()
{
    return rect;
}

void Player::SetPosition(Vector2 pos) {
    rect.x = pos.x;
    rect.y = pos.y;
}

void Player::Update(Level& level) {
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

    if (direction.y == 0 && std::abs(velocity.y) > 0) {
        if (velocity.y < 0)
            dv.y = deceleration * GetFrameTime();
        else if (velocity.y > 0)
            dv.y = -deceleration * GetFrameTime();
    }

    velocity = velocity + dv;

    float currentSpeed = Vector2Length(velocity);
    if (currentSpeed > speed) {
        velocity = Vector2Scale(Vector2Normalize(velocity), speed);
    }

    Vector2 contactPoint, contactNormal;
    float contactTime;

    // Use the player's original collision rect size
    Rectangle playerColRect = rect;
    playerColRect.y -= TILE_WALL_OUTLINE_WIDTH / 2.0;
    playerColRect.x -= TILE_WALL_OUTLINE_WIDTH / 2.0;
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

void Player::Draw(Camera2D &camera) {
    BeginMode2D(camera);
        DrawCircleV(GetPosition(), 20, BLUE);

        // Smaller circles (attached to the body)
        DrawCircleV((Vector2){ GetPosition().x + 30, GetPosition().y }, 10, RED);
        DrawCircleV((Vector2){ GetPosition().x - 30, GetPosition().y }, 10, GREEN);
        DrawCircleV((Vector2){ GetPosition().x, GetPosition().y + 30 }, 10, YELLOW);

        // A rectangle (could represent a shield or additional element)
        DrawRectangleV((Vector2){ GetPosition().x - 15, GetPosition().y - 35 }, (Vector2){ 30, 10 }, PURPLE);
        //DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
    EndMode2D();
}

void Player::ClampToWorldBounds() {
    if (rect.x < 0) rect.x = 0;  // Left boundary
    if (rect.x + rect.width > GetScreenWidth() * N_TILE_WIDTH) 
        rect.x = GetScreenWidth() * N_TILE_WIDTH - rect.width;  // Right boundary

    if (rect.y < 0) rect.y = 0;  // Top boundary
    if (rect.y + rect.height > GetScreenHeight() * N_TILE_HEIGHT) 
        rect.y = GetScreenHeight() * N_TILE_HEIGHT - rect.height;  // Bottom boundary
}

bool Player::CheckCollisionsWithLevel(Level& level, Rectangle rectP, Vector2 velocityP, Vector2& contactPoint, Vector2& contactNormal, float& contactTime) {
    // Check all tiles in the level for collision
    helper::DynamicRect dynamicRect;
    dynamicRect.rect = rectP;
    dynamicRect.vel = velocityP;

    int playerTilePositionX = rectP.x / N_TILE_WIDTH;
    int playerTilePositionY = rectP.y / N_TILE_HEIGHT;

    for (int x = playerTilePositionX - 10; x < playerTilePositionX + 10; x++) {
        for (int y = playerTilePositionY - 10; y < playerTilePositionY + 10; y++) {
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
