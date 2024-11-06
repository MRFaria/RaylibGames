#ifndef LEVEL_H
#define LEVEL_H

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#include "globals.h"
#include "raylib.h"
#include "math.h"
#include <vector>
#include <stdio.h>

class Level
{
public:
    struct Edges
    {
        bool left;
        bool right;
        bool bottom;
        bool top;
    };

    Level(int width, int height);
    ~Level();
    void Cleanup();

    int GetWidth();
    int GetHeight();
    char GetTile(int x, int y);
    char GetFloodTile(int x, int y);
    void Update();
    Texture2D DrawLevelToTexture(Camera2D &camera);
    void LoadRenderTextures();
    void UnloadRenderTextures();
    //void Draw(Camera2D &camera, Texture2D texture);

    void FloodFill(int x, int y);
    float FloodFillStart();

private:
    void InitLevel();
    bool GenerateLevel();
    Edges CheckAdjacentEqual(int x, int y);
    int GetWallTilesWithinNSteps(int x, int y, int n);
    void CellAutomataPass();

private:
    int _width;
    int _height;
    int _passNumber = 0;
    int _screenWidth;
    int _screenHeight;
    RenderTexture2D _glowSpritePassRT;
    RenderTexture2D _glowOutputRT;
    RenderTexture2D _levelRT;
    std::vector<char> _level;
    std::vector<char> _floodFill;
    Shader _shader;
};

#endif
