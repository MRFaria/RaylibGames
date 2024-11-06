#include "Game.h"
#include <iostream>
#include <chrono>

Game::Game(int screenWidth, int screenHeight, Level &level)
    : isRunning(false), level(level)
{
    Init();
}

Game::~Game() {}

void Game::Cleanup()
{
    level.Cleanup();
}

void Game::Run()
{
    while (isRunning)
    {
        float deltaTime = GetFrameTime();
        Update(deltaTime);
        Draw();
    }
}

void Game::Init()
{
    isRunning = true;

    for (int y = 0; y < level.GetHeight(); y++)
    {
        for (int x = 0; x < level.GetWidth(); x++)
        {
            if (level.GetFloodTile(x, y) == TILE_FLOOD)
            {
                player.SetPosition({(float)x*N_TILE_WIDTH + player.GetRect().width*1.2F, (float)y*N_TILE_HEIGHT + player.GetRect().height*1.2F});
                goto EndOfLoop;
            }
        }
    }
EndOfLoop:;
    camera.target = {0.0f, 0.0f}; // Set the initial target position
    camera.offset = {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void Game::CameraUpdate()
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    camera.offset = {width / 2.0f, height / 2.0f};

    if (width > level.GetWidth() * N_TILE_WIDTH)
    {
        camera.offset.x = (width / 2.0f) - (width - level.GetWidth() * N_TILE_WIDTH) / 2.0f;
    }
    if (height > level.GetHeight() * N_TILE_HEIGHT)
    {
        camera.offset.y = (width / 2.0f) - (height - level.GetHeight() * N_TILE_HEIGHT) / 2.0f;
    }

    float smoothingFactor = 0.1f;
    camera.target.x += (player.GetPosition().x - camera.target.x) * smoothingFactor;
    camera.target.y += (player.GetPosition().y - camera.target.y) * smoothingFactor;

    if (camera.target.x < width / 2)
        camera.target.x = width / 2;
    if (camera.target.y < height / 2)
        camera.target.y = height / 2;
    if (camera.target.x > level.GetWidth() * N_TILE_WIDTH - width / 2)
        camera.target.x = level.GetWidth() * N_TILE_WIDTH - width / 2;
    if (camera.target.y > level.GetHeight() * N_TILE_HEIGHT - height / 2)
        camera.target.y = level.GetHeight() * N_TILE_HEIGHT - height / 2;
}
void Game::Update(float delta)
{
    player.Update(level);
    CameraUpdate();
    level.Update();

    if (WindowShouldClose())
    {
        isRunning = false;
    }
}

void Game::Draw()
{
    RenderTexture2D rText = level.LoadDrawTextures(camera);
    Vector2 pos = GetMousePosition();
    std::string fps = "Frame rate is " + std::to_string(GetFPS()) + "\n";
    Vector2 fpsPlacement = {10.0f, 100.0f};

    BeginDrawing();
    ClearBackground(BLACK);

    level.Draw(camera, rText.texture);
    BeginMode2D(camera);
    player.Draw();
    DrawText(fps.c_str(), GetScreenToWorld2D(fpsPlacement, camera).x, GetScreenToWorld2D(fpsPlacement, camera).y, 20, WHITE);
    EndMode2D();
    EndDrawing();

    UnloadRenderTexture(rText);
}
