#include "Player.h"

// Constructor
Player::Player() {
    SetPosition(Vector2{200, 200}); // Center of the screen
    color = BLUE;                   // Player color
    texture = LoadTexture(ASSETS_PATH"textures/rectangle.png");

    playerRect.height = 50;
    playerRect.width = playerRect.height;
}

Player::~Player()
{
    UnloadTexture(texture);
}

Vector2 Player::GetPosition() {
    return Vector2{playerRect.x, playerRect.y};
}

Rectangle Player::GetRect()
{
    return playerRect;
}

void Player::SetPosition(Vector2 pos) {
    playerRect.x = pos.x;
    playerRect.y = pos.y;
}

void Player::Update(Level& level) {

    rotation = rotation + 20*GetFrameTime();
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
    Rectangle playerColRect = playerRect;
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
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        Rectangle destRec = { playerRect.x, playerRect.y, playerRect.width, playerRect.height };
        Vector2 origin = { playerRect.width/2.0, playerRect.height/2.0 }; // Set origin to texture's bottom right corner

        //rotation = 0.0f; // Rotation angle in degrees
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
        //DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
    EndMode2D();
}

void Player::ClampToWorldBounds() {
    if (playerRect.x < 0) playerRect.x = 0;  // Left boundary
    if (playerRect.x + playerRect.width > GetScreenWidth() * N_TILE_WIDTH) 
        playerRect.x = GetScreenWidth() * N_TILE_WIDTH - playerRect.width;  // Right boundary

    if (playerRect.y < 0) playerRect.y = 0;  // Top boundary
    if (playerRect.y + playerRect.height > GetScreenHeight() * N_TILE_HEIGHT) 
        playerRect.y = GetScreenHeight() * N_TILE_HEIGHT - playerRect.height;  // Bottom boundary
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
