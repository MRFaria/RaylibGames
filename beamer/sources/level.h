#ifndef LEVEL_H
#define LEVEL_H

#include "globals.h"
#include "raylib.h"

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

#endif