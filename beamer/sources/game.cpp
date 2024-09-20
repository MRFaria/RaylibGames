#include "raylib.h"
#include "swept-aabb.h"
#include "operators.h"
#include <stdio.h>

class Game {
public:
    Game(int screenWidth, int screenHeight, const char* title)
    : screenWidth(screenWidth), screenHeight(screenHeight), title(title), isRunning(false) 
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
            Update();  // Handle game logic
            Draw();    // Render game visuals
        }
    }


private:
    void Init() 
    {
        InitWindow(screenWidth, screenHeight, title);
        SetTargetFPS(60);
        isRunning = true;
    }


    void Update() 
    {
        if (WindowShouldClose()) 
        {
            isRunning = false;
        }
    }
        

    void Draw() 
    {
        BeginDrawing();
        ClearBackground(BLUE);

        Vector2 vMouse = {float(GetMouseX()), float(GetMouseY())};
        Rectangle r = {100.0f,100.0f, 100.0f, 200.0f};
        Vector2 ray_point = {20.0f, 20.0f};

        Vector2 ray_direction = vMouse - ray_point;
        DrawLineV(ray_point, vMouse, GREEN);

        Vector2 cp, cn;
        float t;

        if (helper::RayVsRect(ray_point, ray_direction, r, cp, cn, t) && t < 1)
        {
            DrawRectangleRec(r, YELLOW);
            DrawCircleV(cp, 10.0, RED);
            DrawLineV(cp, cp+(cn*Vector2{50, 50}), YELLOW);

        }
        else
            DrawRectangleRec(r, WHITE);  
        printf("t hit near is %f\n", t);

        EndDrawing();
    }


    int screenWidth;
    int screenHeight;
    const char* title;
    bool isRunning;
};


int main() {
    Game myGame(800, 450, "My Raylib Game");

    myGame.Run();

    return 0;
}
