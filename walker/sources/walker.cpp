#include "raylib.h"
#include "Animation.h"
#include "Player.h"

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int FPS = 60;
    InitWindow(screenWidth, screenHeight, "PNG Animation Example");
    
    Player player(screenWidth, screenHeight); 

    SetTargetFPS(FPS); // Set target FPS for the application

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        player.Update();

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        player.Draw();
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
