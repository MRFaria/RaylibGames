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

        _camera.target = player.GetPosition();
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
    Game(int screenWidth, int screenHeight, const char* title, Level level)
    : title(title), isRunning(false), level(level)
    {
        Init();
    }


    ~Game() 
    {
        CloseWindow();
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
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);    // Window configuration flags
        InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, title);
        SetTargetFPS(60);
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

        level.Draw();
        player.Draw();

        Vector2 pos = GetMousePosition();
        pos = GetScreenToWorld2D(pos, camera.GetCamera());
        std::string posn = std::to_string((int)pos.x/N_TILE_WIDTH) + std::string(",") + std::to_string((int)pos.y/N_TILE_HEIGHT);
        DrawText(posn.c_str(),10,100, 100, BLACK);

        camera.End();
        EndDrawing();

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            level.FloodFillStart();
        }   
    }
};


int main() {
    SetRandomSeed(std::chrono::steady_clock::now().time_since_epoch().count());
    Level level({100,100});
    Game myGame(800, 600, "My Raylib Game", level);

    myGame.Run();

    return 0;
}
