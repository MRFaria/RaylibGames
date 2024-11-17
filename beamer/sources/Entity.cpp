#include "Entity.h"
#include "math.h"

Entity::Entity(std::string texturePath, Level &level) : _level(level)
{
    std::string path = std::string(ASSETS_PATH) + texturePath;
    _texture = LoadTexture(path.c_str());
}

Entity::~Entity()
{
    UnloadTexture(_texture);
}

void Entity::Update()
{

    _rotation = _rotation + _rotationSpeed * GetFrameTime();
    if (_rotation > 360.0f)
    {
        _rotation = _rotation - 360.0f;
    }
}

void Entity::SetColor(Color color)
{
    _color = color;
}

void Entity::SetScale(float scale)
{
    _scale = scale;
}

void Entity::Draw(Camera2D &camera)
{
    BeginMode2D(camera);
    Rectangle source = {0.0f, 0.0f, (float)_texture.width, (float)_texture.height};
    Rectangle target = GetTextureRect();
    _scale = (float)N_TILE_SIZE / fmax(_texture.width, _texture.height);
    _origin = {target.width / 2.0f, target.height / 2.0f};
    DrawTexturePro(_texture, source, target, _origin, _rotation, _color);
    if (_collision)
    {
        DrawCircleLines(static_cast<int>(_position.x), static_cast<int>(_position.y), GetCollisionObject().radius + 2, RED);        //DrawCircleV(GetCollisionObject().point, GetCollisionObject().radius, RED);
    }
    EndMode2D();
}

Vector2 Entity::GetPosition()
{
    return _position;
}

Circle Entity::GetCollisionObject()
{
    return Circle{_position, fmax(GetTextureRect().width * 0.5f, GetTextureRect().height * 0.5f)};
}

Rectangle Entity::GetTextureRect()
{
    return {_position.x, _position.y, _texture.width * _scale, _texture.height * _scale};
}

void Entity::SetPosition(Vector2 pos)
{
    _position = pos;
}

void Entity::VelocityMove(Vector2 direction)
{

    Vector2 dv = Vector2Normalize(direction) * _acceleration * GetFrameTime();

    if (direction.x * _velocity.x <= 0)
    {
        if (_velocity.x > 0)
        {
            dv.x = -_deceleration * GetFrameTime();
        }
        else if (_velocity.x < 0)
        {
            dv.x = _deceleration * GetFrameTime();
        }
    }
    else
    {
        dv.x = direction.x * _acceleration * GetFrameTime();
    }

    if (direction.y * _velocity.y <= 0)
    {
        if (_velocity.y > 0)
        {
            dv.y = -_deceleration * GetFrameTime();
        }
        else if (_velocity.y < 0)
        {
            dv.y = _deceleration * GetFrameTime();
        }
    }
    else
    {
        dv.y = direction.y * _acceleration * GetFrameTime();
    }

    _velocity = _velocity + dv;

    if (Vector2Length(_velocity) > _speed)
    {
        _velocity = Vector2Scale(Vector2Normalize(_velocity), _speed);
    }
    else if (Vector2Length(_velocity) < _stopThreshold)
    {
        if (direction.y == 0)
            _velocity.y = 0;
        if (direction.x == 0)
            _velocity.x = 0;
    }

    _position = CheckCollisionsWithWorld();
}


Vector2 Entity::CheckCollisionsWithWorld()
{
        _collision = false; // Reset collision flag at the start of the frame
    // Update potential new position
    Vector2 potentialPosition = _position + _velocity * GetFrameTime();

    // Get current and target tile positions in tile space
    Vector2Int currentTile = _position / N_TILE_SIZE; // ::floor(Vector2Scale(_position, 1.0f / N_TILE_SIZE));
    Vector2Int targetTile = potentialPosition / N_TILE_SIZE;

    // Calculate the region of tiles to check for collision
    Vector2Int areaTopLeft = currentTile.min(targetTile) - Vector2Int(1, 1);
    Vector2Int areaBottomRight = currentTile.max(targetTile) + Vector2Int(1, 1);

    // Clamp to world bounds
    areaTopLeft = areaTopLeft.max(Vector2Int(0, 0));
    areaBottomRight = areaBottomRight.min(Vector2Int(_level.GetWidth() - 1, _level.GetHeight() - 1));

    // Collision detection and resolution
    for (int y = areaTopLeft.y; y <= areaBottomRight.y; y++)
    {
        for (int x = areaTopLeft.x; x <= areaBottomRight.x; x++)
        {
            // /printf("%d %d\n", x,y);
            if (_level.GetTile(x, y) == TILE_WALL)
            {

                // Nearest point on the tile to the potential position
                Vector2 nearestPoint;
                nearestPoint.x = std::max(static_cast<float>(x * N_TILE_SIZE), std::min(potentialPosition.x, static_cast<float>((x + 1) * N_TILE_SIZE)));
                nearestPoint.y = std::max(static_cast<float>(y * N_TILE_SIZE), std::min(potentialPosition.y, static_cast<float>((y + 1) * N_TILE_SIZE)));

                Vector2 rayToNearest = nearestPoint - potentialPosition;
                float overlap = GetCollisionObject().radius - Vector2Length(rayToNearest);
                if (overlap > 0)
                {
                    // Collision detected
                    Vector2 correction = Vector2Scale(Vector2Normalize(rayToNearest), overlap);
                    potentialPosition = Vector2Subtract(potentialPosition, correction);

                    // Update collision variables
                    _collision = true;
                    _nearestPoint = nearestPoint;
                }
                else
                {
                    // No collision, but do not reset for all iterations
                    if (!_collision)
                    {
                        _collision = false;
                        _nearestPoint = _position; // Reset only if no collision detected anywhere
                    }
                }
            }
        }
    }

    return potentialPosition;
}
