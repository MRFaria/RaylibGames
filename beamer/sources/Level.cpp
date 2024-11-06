#include "Level.h"

// Constructor
Level::Level(int width, int height) : _width(width), _height(height)
{
    int attempts = 0;
    InitLevel();
    auto goodLevel = GenerateLevel();
    while (!goodLevel && attempts < 5)
    {
        for (int i = 0; i < _width * _height; i++)
        {
            _level[i] = TILE_WALL;
            _floodFill[i] = TILE_WALL;
        }
        printf("Generating level. attempt %d\n", attempts);
        attempts++;
        goodLevel = GenerateLevel();
    }
    if (!goodLevel)
    {
        printf("Failed to generate a nice level\n");
        exit(1);
    }

    _shader = LoadShader(0, TextFormat(ASSETS_PATH "/shaders/glsl%i/shader.fs", GLSL_VERSION));
}

// Destructor
Level::~Level()
{
}

void Level::Cleanup()
{
    UnloadShader(_shader);
}

int Level::GetWidth()
{
    return _width;
}

int Level::GetHeight()
{
    return _height;
}

char Level::GetTile(int x, int y)
{
    if ((y >= 0 && y < _height) && (x >= 0 && x < _width))
        return _level[y * _width + x];
    else
        return TILE_WALL;
}

char Level::GetFloodTile(int x, int y)
{
    if ((y >= 0 && y < _height) && (x >= 0 && x < _width))
        return _floodFill[y * _width + x];
    else
        return TILE_WALL;
}

void Level::Update()
{
    // Update logic if needed
}

RenderTexture2D Level::LoadDrawTextures(Camera2D &camera)
{
    int startX = fmax(0, (int)(camera.target.x - GetScreenWidth() / 2) / N_TILE_WIDTH);
    int endX = fmin(GetWidth(), (int)(camera.target.x + GetScreenWidth() / 2) / N_TILE_WIDTH + 1);
    int startY = fmax(0, (int)(camera.target.y - GetScreenHeight() / 2) / N_TILE_HEIGHT);
    int endY = fmin(GetHeight(), (int)(camera.target.y + GetScreenHeight() / 2) / N_TILE_HEIGHT + 1);

    RenderTexture2D glowSpritePass = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    RenderTexture2D glowOutput = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    int colDiffuseLoc = GetShaderLocation(_shader, "colDiffuse");
    int resolutionLoc = GetShaderLocation(_shader, "resolution");
    SetShaderValue(_shader, colDiffuseLoc, (float[4]){1.0f, 1.0f, 1.0f, 1.0f}, SHADER_UNIFORM_VEC4);
    SetShaderValue(_shader, resolutionLoc, (float[2]){(float)GetScreenWidth(), (float)GetScreenHeight()}, SHADER_UNIFORM_VEC2);

    BeginTextureMode(glowSpritePass);
    BeginMode2D(camera);
    ClearBackground(BLANK);

    for (int x = startX; x < endX; x++)
    {
        for (int y = startY; y < endY; y++)
        {
            float tileX = x * N_TILE_WIDTH;
            float tileY = y * N_TILE_HEIGHT;
            auto sTileId = GetTile(x, y);
            if (sTileId == TILE_WALL)
            {
                Edges adjacentTiles = CheckAdjacentEqual(x, y);
                Color outlineColor = RED;
                if (!adjacentTiles.top)
                    DrawLineEx({tileX, tileY}, {tileX + N_TILE_WIDTH, tileY}, TILE_WALL_OUTLINE_WIDTH, outlineColor);
                if (!adjacentTiles.bottom)
                    DrawLineEx({tileX, tileY + N_TILE_HEIGHT}, {tileX + N_TILE_WIDTH, tileY + N_TILE_HEIGHT}, TILE_WALL_OUTLINE_WIDTH, outlineColor);
                if (!adjacentTiles.left)
                    DrawLineEx({tileX, tileY}, {tileX, tileY + N_TILE_HEIGHT}, TILE_WALL_OUTLINE_WIDTH, outlineColor);
                if (!adjacentTiles.right)
                    DrawLineEx({tileX + N_TILE_WIDTH, tileY}, {tileX + N_TILE_WIDTH, tileY + N_TILE_HEIGHT}, TILE_WALL_OUTLINE_WIDTH, outlineColor);
            }
        }
    }

    EndMode2D();
    EndTextureMode();

    Rectangle screen_rect = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    Rectangle texture_rect = {0, 0, (float)glowSpritePass.texture.width, (float)glowSpritePass.texture.height};

    BeginTextureMode(glowOutput);
    BeginShaderMode(_shader);
    DrawTexturePro(glowSpritePass.texture, texture_rect, screen_rect, {0, 0}, 0, WHITE);
    EndShaderMode();
    EndTextureMode();

    UnloadRenderTexture(glowSpritePass);

    return glowOutput;
}

void Level::Draw(Camera2D &camera, Texture2D texture)
{
    BeginMode2D(camera);

    int startX = fmax(0, (int)(camera.target.x - GetScreenWidth() / 2) / N_TILE_WIDTH);
    int endX = fmin(GetWidth(), (int)(camera.target.x + GetScreenWidth() / 2) / N_TILE_WIDTH + 1);
    int startY = fmax(0, (int)(camera.target.y - GetScreenHeight() / 2) / N_TILE_HEIGHT);
    int endY = fmin(GetHeight(), (int)(camera.target.y + GetScreenHeight() / 2) / N_TILE_HEIGHT + 1);

    for (int x = startX; x < endX; x++)
    {
        for (int y = startY; y < endY; y++)
        {
            float tileX = x * N_TILE_WIDTH;
            float tileY = y * N_TILE_HEIGHT;
            auto sTileId = GetTile(x, y);
            if (sTileId == TILE_WALL)
            {
                DrawRectangle(tileX, tileY, N_TILE_WIDTH, N_TILE_HEIGHT, DARKGREEN);
            }
        }
    }

    EndMode2D();

    Rectangle screen_rect = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    Rectangle texture_rect = {0, 0, (float)texture.width, (float)texture.height};

    //BeginBlendMode(BLEND_ADD_COLORS);
    DrawTexturePro(texture, texture_rect, screen_rect, {0, 0}, 0, WHITE);
    //EndBlendMode();
}

void Level::InitLevel()
{
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            _level.push_back(TILE_WALL);
            _floodFill.push_back(TILE_WALL);
        }
    }
}

bool Level::GenerateLevel()
{
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            if (x == 0 || x == _width - 1 || y == 0 || y == _height - 1)
                _level[y * _width + x] = TILE_WALL;
            else if (GetRandomValue(0, 100) < WALL_CHANCE)
                _level[y * _width + x] = TILE_WALL;
            else
                _level[y * _width + x] = TILE_EMPTY;
        }
    }

    for (int i = 0; i <= NUM_PASSES; i++)
        CellAutomataPass();

    return FloodFillStart() >= 60;
}

Level::Edges Level::CheckAdjacentEqual(int x, int y)
{
    Edges edges = {false, false, false, false};
    if (GetTile(x - 1, y) == GetTile(x, y))
        edges.left = true;
    if (GetTile(x, y - 1) == GetTile(x, y))
        edges.top = true;
    if (GetTile(x + 1, y) == GetTile(x, y))
        edges.right = true;
    if (GetTile(x, y + 1) == GetTile(x, y))
        edges.bottom = true;
    return edges;
}

int Level::GetWallTilesWithinNSteps(int x, int y, int n)
{
    int count = 0;
    for (int iy = y - n; iy <= y + n; iy++)
    {
        for (int ix = x - n; ix <= x + n; ix++)
        {
            if (ix == x && iy == y)
                continue;
            if (GetTile(ix, iy) == TILE_WALL)
                count++;
        }
    }
    return count;
}

void Level::CellAutomataPass()
{
    // Create a copy of the current level
    std::vector<char> _buffer = _level;

    // Loop through the grid
    for (int y = 0; y < _height; y++) // Change from <= to <
    {
        for (int x = 0; x < _width; x++) // Change from <= to <
        {
            // Determine if the current tile is a wall
            if (GetTile(x, y) == TILE_WALL)
            {
                // Apply the cell automata rules
                _buffer[y * _width + x] = (GetWallTilesWithinNSteps(x, y, 1) < DEATH_LIMIT) ? TILE_EMPTY : TILE_WALL;
            }
            else
            {
                _buffer[y * _width + x] = (GetWallTilesWithinNSteps(x, y, 1) > BIRTH_LIMIT) ? TILE_WALL : TILE_EMPTY;
            }
        }
    }

    // Update the level with the new values
    _level = _buffer;
}

void Level::FloodFill(int x, int y)
{
    if (x < 0 || x >= _width || y < 0 || y >= _height || _floodFill[y * _width + x] == TILE_FLOOD)
        return;

    _floodFill[y * _width + x] = TILE_FLOOD;
    Edges edges = CheckAdjacentEqual(x, y);

    if (edges.top)
        FloodFill(x, y - 1);
    if (edges.bottom)
        FloodFill(x, y + 1);
    if (edges.right)
        FloodFill(x + 1, y);
    if (edges.left)
        FloodFill(x - 1, y);
}

float Level::FloodFillStart()
{
    _floodFill = _level;
    int attempts = 0;
    int y = GetRandomValue(_height / 3, 2 * _height / 3);
    int x = GetRandomValue(_width / 3, 2 * _width / 3);
    auto tile = GetTile(x, y);

    while (tile != TILE_EMPTY && attempts < 50)
    {
        x = GetRandomValue(_width / 3, 2 * _width / 3);
        y = GetRandomValue(_height / 3, 2 * _height / 3);
        tile = GetTile(x, y);
        attempts++;
    }

    if (tile != TILE_EMPTY)
        return 0.0;

    FloodFill(x, y);
    int sizeContinuousLevel = 0;
    for (int i = 0; i < _height * _width; i++)
    {
        if (_floodFill[i] == TILE_FLOOD)
            sizeContinuousLevel++;
    }

    printf("Size is of flood %lf\n", sizeContinuousLevel * 100.0 / (_width * _height));
    return sizeContinuousLevel * 100.0 / (_width * _height);
}
