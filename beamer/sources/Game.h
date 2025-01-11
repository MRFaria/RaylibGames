#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Level.h"
#include "Player.h"
#include <string>

class Game {
public:
    Game(int screenWidth, int screenHeight, Level &level);
    ~Game();
    void Run();
    void Cleanup();

private:
    const char* title;
    bool isRunning;
    Level level;
    Camera2D camera;
    Player player;
    std::vector<Entity> entities;

    void InitPlayer();
    void Init();
    void Update(float delta);
    void Draw();
    void CameraUpdate();
    void DrawFramerate(Camera2D &camera);
};

#endif // GAME_H
