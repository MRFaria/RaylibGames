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
    
    //generic texture for rectangles
    Texture2D _rectangle;
    // shader stuff
    Shader _shader;
    int _shaderTimeLoc;
    int _shaderResolutionLoc;
    int _shaderBlurRadiusLoc;
    int _textureLoc;
    Vector4 _colDiffuse;
    Vector2 _resolution;
    float _time;
    float _blurRadius;


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


        _rectangle = LoadTexture(ASSETS_PATH"/textures/rectangle.png");

        _shader = LoadShader(0, TextFormat(ASSETS_PATH"/shaders/glsl%i/test.fs", GLSL_VERSION));
        _time = 0.0;
        _resolution  = {(float)GetScreenWidth(), (float)GetScreenHeight()};
        _blurRadius = 10.0;

        _shaderTimeLoc = GetShaderLocation(_shader, "u_time");
        _shaderResolutionLoc = GetShaderLocation(_shader, "u_resolution");
        _shaderBlurRadiusLoc = GetShaderLocation(_shader, "u_blurRadius");

        SetShaderValue(_shader, _shaderTimeLoc, &_time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(_shader, _shaderResolutionLoc, &_resolution, SHADER_UNIFORM_VEC2);
        SetShaderValue(_shader, _shaderBlurRadiusLoc, &_blurRadius, SHADER_UNIFORM_FLOAT);
    }
    
    void Cleanup()
    {
        UnloadShader(_shader);
        UnloadTexture(_rectangle);
    }

    ~Level()
    {
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
        _time += GetFrameTime()/4.0;
        SetShaderValue(_shader, _shaderTimeLoc, &_time, SHADER_UNIFORM_FLOAT);
    }



    void Draw(Camera2D &camera)
    {
        int startX = fmax(0, (int)(camera.target.x - GetScreenWidth() / 2) / N_TILE_WIDTH);
        int endX = fmin(GetWidth(), (int)(camera.target.x + GetScreenWidth() / 2) / N_TILE_WIDTH + 1);
        int startY = fmax(0, (int)(camera.target.y - GetScreenHeight() / 2) / N_TILE_HEIGHT);
        int endY = fmin(GetHeight(), (int)(camera.target.y + GetScreenHeight() / 2) / N_TILE_HEIGHT + 1);
        // Loop through the tiles and only draw those within the camera view
        for (int x = startX; x < endX; x++)
        {
            for (int y = startY; y < endY; y++)
            {
                float tileX = x * N_TILE_WIDTH;
                float tileY = y * N_TILE_HEIGHT;
                {
                    auto sTileId = GetTile(x, y);
                    if (sTileId == TILE_WALL)
                    {

                        Color color = (Color){ 0, 228, 48, 30 } ;
                        DrawRectangle(tileX, tileY, N_TILE_WIDTH, N_TILE_HEIGHT, color);
                        Level::Edges edges = CheckAdjacentEqual(x,y);
                        BeginShaderMode(_shader);
                        if (!edges.top) {
                            Rectangle sourceRect = { 0, 0, N_TILE_WIDTH, TILE_WALL_OUTLINE_WIDTH };
                            DrawTextureRec(_rectangle, sourceRect, { tileX, tileY - TILE_WALL_OUTLINE_WIDTH }, GREEN);
                        }

                        if (!edges.bottom) {
                            Rectangle sourceRect = { 0, 0, N_TILE_WIDTH, TILE_WALL_OUTLINE_WIDTH };
                            // Adjust the position for the bottom edge to align precisely with the bottom boundary
                            DrawTextureRec(_rectangle, sourceRect, { tileX, tileY + N_TILE_HEIGHT }, GREEN);
                        }

                        if (!edges.left) {
                            Rectangle sourceRect = { 0, 0, TILE_WALL_OUTLINE_WIDTH, N_TILE_HEIGHT };
                            DrawTextureRec(_rectangle, sourceRect, { tileX - TILE_WALL_OUTLINE_WIDTH, tileY }, GREEN);
                        }

                        if (!edges.right) {
                            Rectangle sourceRect = { 0, 0, TILE_WALL_OUTLINE_WIDTH, N_TILE_HEIGHT };
                            // Adjust the position for the right edge to align precisely with the right boundary
                            DrawTextureRec(_rectangle, sourceRect, { tileX + N_TILE_HEIGHT, tileY }, GREEN);
                        }  
                        EndShaderMode();
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

#endif