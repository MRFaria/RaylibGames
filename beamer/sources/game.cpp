#include "raylib.h"
#include "swept-aabb.h"
#include "operators.h"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>


const char TILE_WALL = '#';
const char TILE_EMPTY = '.';
const char TILE_FLOOD = 'F';

const int N_TILE_WIDTH = 64;
const int N_TILE_HEIGHT = N_TILE_WIDTH;
const int WALL_CHANCE = 45;
const int DEATH_LIMIT = 4; //if alive and num tiles around less
const int BIRTH_LIMIT = 4; //if dead and num tiles around more
const int NUM_PASSES = 10;

const int INITIAL_SCREEN_HEIGHT = 600;
const int INITIAL_SCREEN_WIDTH = 800;


class Player {
    public:
    Vector2 position; // Player's position
    float speed;      // Player's movement speed
    Color color;      // Color of the player
    int width;


    Player()
    {
        // Initialize player properties
        position = (Vector2){400, 300}; // Center of the screen
        speed = 400.0f;                 // Movement speed
        color = BLUE;                   // Player color
        width = 25;
    }
    void Update()
    {
        // Movement logic
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) position.x += speed * GetFrameTime();
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) position.x -= speed * GetFrameTime();
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) position.y -= speed * GetFrameTime();
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) position.y += speed * GetFrameTime();

        // Clamp player's position to the world boundaries
        if (position.x < 0) position.x = 0;  // Left boundary
        if (position.x + width > GetScreenWidth() * N_TILE_WIDTH) 
            position.x = GetScreenWidth() * N_TILE_WIDTH - width;  // Right boundary

        if (position.y < 0) position.y = 0;  // Top boundary
        if (position.y + width > GetScreenHeight() * N_TILE_HEIGHT) 
            position.y = GetScreenHeight() * N_TILE_HEIGHT - width;  // Bottom boundary
    }
    void Draw()
    {
        DrawCircle(position.x, position.y, width, color);
    }
};


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
    std::vector<char> flood_fill;
    private:
    int _width;
    int _height;
    int _passNumber = 0;
    std::vector<char> _level; 


    public:
    Level(int width, int height) : _width(width), _height(height)
    {
        int attempts = 0;
        InitLevel();
        auto goodLevel = GenerateLevel();
        while(!goodLevel && attempts < 5)
        {
            for(int i=0; i<_width*_height; i++)
            {
                _level[i] = TILE_WALL;
                flood_fill[i] = TILE_WALL;
            }
            printf("Generating level. attempt %d\n", attempts);
            attempts++;
            goodLevel = GenerateLevel();
        }
        if(!goodLevel)
        {
            printf("Failed to generate a nice level\n");
            exit(1);
        }
    }

    int GetWidth()
    {
        return _width;
    }

    int GetHeight()
    {
        return _height;
    }

    char GetTile(int x, int y)
    {
        if((y>=0 and y<_height) and (x>=0 and x<_width)) 
            return _level[y*_width + x];
        else
            return TILE_WALL;
    }

    void Update()
    {
        //
    }

    void Draw()
    {
        // Loop through the tiles and only draw those within the camera view
        for (int x = 0; x < _width; x++)
        {
            for (int y = 0; y < _height; y++)
            {
                float tileX = x * N_TILE_WIDTH;
                float tileY = y * N_TILE_HEIGHT;
                {
                    auto sTileId = GetTile(x, y);
                    if (sTileId == TILE_WALL)
                    {
                        DrawRectangle(tileX, tileY, N_TILE_WIDTH, N_TILE_HEIGHT, BROWN);
                        Level::Edges edges = CheckAdjacentEqual(x,y);
                        if (!edges.top)
                            DrawLineEx({tileX, tileY}, {tileX + N_TILE_WIDTH, tileY}, 3, GREEN);
                        if (!edges.bottom)
                            DrawLineEx({tileX, tileY+N_TILE_HEIGHT}, {tileX + N_TILE_WIDTH, tileY+N_TILE_HEIGHT}, 3, GREEN);
                        if (!edges.left)
                            DrawLineEx({tileX, tileY}, {tileX, tileY+N_TILE_HEIGHT}, 3, GREEN);
                        if (!edges.right)
                            DrawLineEx({tileX+N_TILE_WIDTH, tileY}, {tileX+N_TILE_WIDTH, tileY+N_TILE_HEIGHT}, 3, GREEN);
                    }
                    if (sTileId == TILE_FLOOD)
                    {
                        DrawRectangle(tileX, tileY, N_TILE_WIDTH, N_TILE_HEIGHT, BLUE);                       
                    }

                }
            }
        }
    }

    private:

    void InitLevel()
    {
        for(int y=0;y<_height;y++)
        {
            for(int x=0;x<_width;x++)           
            {
                _level.push_back(TILE_WALL);
                flood_fill.push_back(TILE_WALL);
            }
        }

    }
    bool GenerateLevel()
    {
        for(int y=0;y<_height;y++)
        {
            for(int x=0;x<_width;x++)  
            { 
            if (x == 0 || x == _width -1 || y == 0 || y == _height-1)
                _level[y*_width + x] = TILE_WALL;
            else if(GetRandomValue(0, 100) < WALL_CHANCE)
                _level[y*_width + x] = TILE_WALL;
            else
                _level[y*_width + x] = TILE_EMPTY;
            }
        }
        for (int i=0; i<=NUM_PASSES; i++)
            CellAutomataPass();

        if (FloodFillStart() < 60) {
            return false;
        }

        return true;

    }

    Edges CheckAdjacentEqual(int x, int y)
    {
        Edges edges = {false, false, false, false};
        if(GetTile(x-1, y) == GetTile(x,y))
            edges.left = true;
        if(GetTile(x, y-1) == GetTile(x,y))
            edges.top = true;        
        if(GetTile(x+1, y) == GetTile(x,y))
            edges.right = true;
        if(GetTile(x, y+1) == GetTile(x,y))
            edges.bottom = true;
        return edges;
    }

    int GetWallTilesWithinNSteps(int x, int y, int n)
    {
        int count = 0;
        for(int iy=y-n; iy<=y+n; iy++)
        {
            for(int ix=x-n; ix<=x+n; ix++)           
            {
                if (ix == x && iy ==y) {
                    // do nothing
                }
                else if (GetTile(ix,iy) == TILE_WALL)
                {
                    count++;
                }
            }
        }
        return count;
    }

    void CellAutomataPass()
    {
        std::vector<char> _buffer;
        _buffer.resize(_level.size());
        for (size_t i = 0; i < _level.size(); ++i) {
            _buffer.push_back(_level[i]);
        }

        for(int y=0; y<=_height; y++)
        {
            for(int x=0; x<=_width; x++)
            {
                if (GetTile(x,y) == TILE_WALL) //alive
                {
                    if (GetWallTilesWithinNSteps(x,y,1) < DEATH_LIMIT)
                    {
                        _buffer[y*_width+x] = TILE_EMPTY;
                    }
                    else
                    {
                         _buffer[y*_width+x] = TILE_WALL;                       
                    }
                }
                else //dead
                {
                    if (GetWallTilesWithinNSteps(x,y,1) > BIRTH_LIMIT)
                    {
                        _buffer[y*_width+x] = TILE_WALL;
                    }
                    else
                    {
                         _buffer[y*_width+x] = TILE_EMPTY;                       
                    }
                }
            }
        }
        _level = _buffer;
    }
    public:
    
   
    void FloodFill(int x, int y)
    {
        // out of bounds
        if (x < 0 || x >= _width || y < 0 || y >= _height) {
            return;
        }

        if (flood_fill[y * _width + x] == TILE_FLOOD) {
            return;
        }

        flood_fill[y * _width + x] = TILE_FLOOD;

        Edges edges = CheckAdjacentEqual(x, y);

        // Recursive
        if (edges.top) {
            FloodFill(x, y - 1);
        }
        if (edges.bottom) {
            FloodFill(x, y + 1);
        }
        if (edges.right) {
            FloodFill(x + 1, y);
        }
        if (edges.left) {
            FloodFill(x - 1, y);
        }
    }


    float FloodFillStart()
    {
        flood_fill = _level;
        int attempts = 0;
        int y = GetRandomValue(_height/3, 2*_height/3);
        int x = GetRandomValue(_width/3, 2*_width/3);
        auto tile = GetTile(x,y);
        while(tile != TILE_EMPTY && attempts < 50) 
        {
            x = GetRandomValue(_width/3, 2*_width/3);
            y = GetRandomValue(_height/3, 2*_height/3);
            tile = GetTile(x,y);
            attempts++;
        }
        // return early, can't find an empty tile
        if (tile != TILE_EMPTY)
            return 0.0;

        FloodFill(x,y);
        int sizeContinuousLevel = 0;
        for (int i =0; i<_height*_width; i++)
        {
            if (flood_fill[i] == TILE_FLOOD)
                sizeContinuousLevel++;
        }
        printf("Size is of flood %lf\n", sizeContinuousLevel*100.0/(_width*_height));
        return sizeContinuousLevel*100.0/(_width*_height);

    }

};


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

        _camera.target = player.position;
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
                    player.position = {(float)x*N_TILE_WIDTH + player.width*1.2F, (float)y*N_TILE_HEIGHT + player.width*1.2F};
                    goto EndOfLoop;
                }
            }
        }
        EndOfLoop:;

    }

    void Update(float delta) 
    { 
        player.Update();
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
