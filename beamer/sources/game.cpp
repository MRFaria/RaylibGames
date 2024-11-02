#include "raylib.h"
#include "swept-aabb.h"
#include "helpers.h"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <player.h>
#include "globals.h"



class GameCamera {
    Camera2D _camera;
    public:
    GameCamera()
    {
        //camera.target = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
        _camera.rotation = 0.0f;
        _camera.zoom = 1.0f;
    }

    Camera2D GetCamera()
    {
        return _camera;
    }

    void Update(Level &level, Player &player)
    {
        int width = GetScreenWidth();
        int height = GetScreenHeight();

        _camera.offset = (Vector2){ width / 2.0f, height / 2.0f };
        
        if (width > level.GetWidth() * N_TILE_WIDTH) {
            _camera.offset.x = (width / 2.0f) - (width - level.GetWidth()*N_TILE_WIDTH) / 2.0f;
        }

        if (height > level.GetHeight() * N_TILE_HEIGHT) {
            _camera.offset.y = (width / 2.0f) - (height - level.GetHeight() * N_TILE_HEIGHT) / 2.0f;
        }

        float smoothingFactor = 0.1f; // Adjust this for smoothness
        _camera.target.x += (player.GetPosition().x - _camera.target.x) * smoothingFactor;
        _camera.target.y += (player.GetPosition().y - _camera.target.y) * smoothingFactor;
        // Bound camera target within level limits
        if (_camera.target.x < width / 2)
            _camera.target.x = width / 2;

        if (_camera.target.y < height / 2)
            _camera.target.y = height / 2;

        if (_camera.target.x > level.GetWidth() * N_TILE_WIDTH - width / 2)
            _camera.target.x = level.GetWidth() * N_TILE_WIDTH - width / 2;

        if (_camera.target.y > level.GetHeight() * N_TILE_HEIGHT - height / 2)
            _camera.target.y = level.GetHeight() * N_TILE_HEIGHT - height / 2;   
    }

    void Begin()
    {
        BeginMode2D(_camera);
    }

    void End()
    {
        EndMode2D();
    }
};


class Game {
    public:
    Game(int screenWidth, int screenHeight, Level& level)
    : isRunning(false), level(level)
    {
        Init();
    }


    ~Game() 
    {

    }

    void Cleanup()
    {
        level.Cleanup();
    }

    void Run() 
    {
        while (isRunning) 
        {
            float deltaTime = GetFrameTime();
            Update(deltaTime);
            Draw();
        }
    }


    private:

    const char* title;
    bool isRunning;

    Level level;
    GameCamera camera;
    Player player;

    void Init() 
    {
        isRunning = true;

        for (int y = 0; y<level.GetHeight(); y++)
        {
            for (int x = 0; x<level.GetWidth(); x++)
            {
                if (level.flood_fill[y*level.GetWidth()+x] == TILE_FLOOD)
                {
                    player.rect.x = (float)x*N_TILE_WIDTH + player.rect.width*1.2F;
                    player.rect.y = (float)y*N_TILE_HEIGHT + player.rect.height*1.2F;
                    goto EndOfLoop;
                }
            }
        }
        EndOfLoop:;

    }

    void Update(float delta) 
    { 
        player.Update(level);
        camera.Update(level, player);
        level.Update();

        if (WindowShouldClose()) 
        {
            isRunning = false;
        }
    }
        

    void Draw() 
    {

        BeginDrawing();
        ClearBackground(BLACK);
        camera.Begin();
        Camera2D cam = camera.GetCamera();
        level.Draw(cam);
        player.Draw();

        Vector2 pos = GetMousePosition();

        std::string fps = std::string("Frame rate is ") + std::to_string(GetFPS()) + std::string("\n");
        Vector2 fpsPlacement = {10.0,100.0};
        DrawText(fps.c_str(),GetScreenToWorld2D(fpsPlacement, cam).x, GetScreenToWorld2D(fpsPlacement, cam).y, 20, WHITE);

        camera.End();
        EndDrawing();
    }
};


int main() {
    SetRandomSeed(std::chrono::steady_clock::now().time_since_epoch().count());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);    // Window configuration flags
    InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Beamer");
    SetTargetFPS(60);

    Level level({100,100});
    Game myGame(800, 600, level);

    myGame.Run();

    CloseWindow();
    myGame.Cleanup();
    return 0;
}
