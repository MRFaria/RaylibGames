#include "raylib.h"
#include "Game.h"
#include <chrono>

int main() {
    SetRandomSeed(std::chrono::steady_clock::now().time_since_epoch().count());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Beamer");
    SetTargetFPS(60);

    Level level({100, 100});
    Game myGame(800, 600, level);

    myGame.Run();

    CloseWindow();
    myGame.Cleanup();
    return 0;
}
