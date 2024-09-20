#include "raylib.h"




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
        Rectangle s = {vMouse.x, vMouse.y, 40.0f, 80.0f};
        
        DrawRectangleLinesEx(s, 5.0f, GREEN);

        if (CheckCollisionRecs(r,s))
            DrawRectangleLinesEx(r, 5.0f, YELLOW);
        else
            DrawRectangleLinesEx(r, 5.0f, WHITE);

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
