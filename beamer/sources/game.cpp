#include "raylib.h"
#include "swept-aabb.h"
#include "operators.h"
#include <string>
#include <iostream>
#include <stdexcept>

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
            float deltaTime = GetFrameTime();
            Update(deltaTime);  // Handle game logic
            Draw();    // Render game visuals
        }
    }

    wchar_t GetTile(int x, int y)
    {
        if (x>=0 && x < nLevelWidth && y >= 0 && y < nLevelHeight)
            return sLevel[y * nLevelWidth + x];
        else 
            return L' ';
    }

    void SetTile(int x, int y, wchar_t c)
    {
        if (x>=0 && x < nLevelWidth && y >= 0 && y < nLevelHeight)
            sLevel[y * nLevelWidth + x] = c;
    }

    void InitTileMap()
    {
        bool widthDetected = false;
        for (size_t i = 1; i < sLevel.size(); i++)
        {
            if (sLevel[i] == L'\n')
            {
                nLevelHeight++;
                widthDetected = true;
            }
            else if(!widthDetected)
            {
                nLevelWidth++;
            }
        }
        for (auto i = sLevel.begin(); i != sLevel.end();)
        {
            if (*i == L'\n')  
            {
                i = sLevel.erase(i);  
            }
            else
            {
                ++i;
            }
        }
        if (sLevel.size() != nLevelHeight * nLevelWidth) 
        {
            throw std::runtime_error("Error: Tilemap size does not match the expected dimensions!");
        }
    }

private:
    std::wstring sLevel = 
LR"(
.....#.......
.....#.......
.....#.......
.....#.......
.....#.......
.....#.......
)";

    int nLevelWidth = 0;
    int nLevelHeight = 0;
    int nTileWidth = 64;
    int nTileHeight = 64;
    float fCameraPosX = 0.0f;
    float fCameraPosY = 0.0f;
    int nVisibleTilesX = 0;
    int nVisibleTilesY = 0;
    Vector2 playerPos = Vector2{0.0f, 0.0f};
    Camera2D camera;

    void Init() 
    {

        InitTileMap();
        InitWindow(screenWidth, screenHeight, title);
        SetTargetFPS(60);
        isRunning = true;


        camera.rotation = 0.0f;
        camera.zoom = 1.0f;

    }

    void Update(float delta) 
    { 
	    nVisibleTilesX = screenWidth / nTileWidth;
		nVisibleTilesY = screenHeight / nTileHeight;
        camera.target = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
 
        camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
        playerPos.x = screenWidth / 2;
        playerPos.y = screenWidth /2 ;

        if (IsKeyDown(KEY_RIGHT)) playerPos.x += nTileWidth;
        else if (IsKeyDown(KEY_LEFT)) playerPos.x -= nTileWidth;

        // Camera reset (zoom and rotation)
        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            camera.rotation = 0.0f;
        }
            // Camera zoom controls
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;
        if (WindowShouldClose()) 
        {
            isRunning = false;
        }
    }
        

    void Draw() 
    {
        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);
        int total = 0;
        int tilesToDraw;

        // Calculate the bounds of the camera view in world space
        // note, camera.target is the camera in the world coordinates
        float camLeft = camera.target.x - (screenWidth * 0.5f / camera.zoom);
        float camRight = camera.target.x + (screenWidth * 0.5f / camera.zoom);
        float camTop = camera.target.y - (screenHeight * 0.5f / camera.zoom);
        float camBottom = camera.target.y + (screenHeight * 0.5f / camera.zoom);


        // Loop through the tiles and only draw those within the camera view
        for (int x = 0; x < nLevelWidth; x++)
        {
            for (int y = 0; y < nLevelHeight; y++)
            {
                float tileX = x * nTileWidth;
                float tileY = y * nTileHeight;

                // Check if the tile is within the camera bounds before drawing (draw two extra tile layers outside of camera)
                if (tileX + 2*nTileWidth >= camLeft && tileX <= camRight &&
                    tileY + 2*nTileHeight >= camTop && tileY <= camBottom)
                {
                    auto sTileId = GetTile(x, y);
                    if (sTileId == L'#')
                    {
                        DrawRectangle(tileX, tileY, nTileWidth, nTileHeight, BROWN);
                    }
                    else 
                    {
                        DrawRectangleLines(tileX, tileY, nTileWidth, nTileHeight, BLACK);
                    }
                }
            }
        }    
        EndMode2D();
        DrawRectangle(0, 0, nTileWidth, nTileHeight, YELLOW);  
        EndDrawing();
    }


    int screenWidth;
    int screenHeight;
    const char* title;
    bool isRunning;
};


int main() {
    Game myGame(800, 600, "My Raylib Game");

    myGame.Run();

    return 0;
}
