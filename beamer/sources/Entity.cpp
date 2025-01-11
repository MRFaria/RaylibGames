#include "Entity.h"
#include "math.h"

// THIS CLASS needs to be optimised, as spawning a lot of them slows down the game. 
// is it the collision?
Entity::Entity(std::string texturePath, Level &level) : _level(level)
{
    std::string path = std::string(ASSETS_PATH) + texturePath;
    _texture = LoadTexture(path.c_str());
    EntityNumber = -1;
}

Entity::Entity(Entity&& other) noexcept 
    : _texture(other._texture), // Move the texture
      _level(other._level),     // Reference remains valid
      _position(other._position),
      _velocity(other._velocity),
      _rotation(other._rotation),
      _rotationSpeed(other._rotationSpeed),
      _scale(other._scale),
      _color(other._color),
      _acceleration(other._acceleration),
      _deceleration(other._deceleration),
      _speed(other._speed),
      _stopThreshold(other._stopThreshold),
      _collision(other._collision),
      _nearestPoint(other._nearestPoint),
      _origin(other._origin),
      EntityNumber(other.EntityNumber)

{
    // Leave the source object in a valid state
    other._texture = { 0 }; // Reset texture to an empty state
}

Entity::~Entity()
{
    UnloadTexture(_texture);
}

void Entity::Update(std::vector<Entity> &entities)
{

    _rotation = _rotation + _rotationSpeed * GetFrameTime();
    if (_rotation > 360.0f)
    {
        _rotation = _rotation - 360.0f;
    }

    VelocityMove(Vector2{0.0, 0.0}, entities);
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
        DrawCircle(static_cast<int>(_position.x), static_cast<int>(_position.y), GetCollisionObject().radius, {230,31,35,100});        //DrawCircleV(GetCollisionObject().point, GetCollisionObject().radius, RED);
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

void Entity::VelocityMove(Vector2 direction, std::vector<Entity> &otherEntities)
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

    _position = CheckCollisionsWithEntities(otherEntities);
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
                        _nearestPoint = _position; // Reset only if no collision detected anywhere
                    }
                }
            }
        }
    }

    return potentialPosition;
}

Vector2 Entity::CheckCollisionsWithEntities(std::vector<Entity> &otherEntities)
{
    Vector2 newPosition = _position + _velocity * GetFrameTime();
    Vector2 adjustedVelocity = _velocity; // Start with the original velocity

    for (int iEntity = 0; iEntity < otherEntities.size(); iEntity++)
    {
        printf("Entity number %d, other entity number %d\n", EntityNumber, otherEntities[iEntity].EntityNumber);
        if (EntityNumber == otherEntities[iEntity].EntityNumber)
        {
            continue; // Skip self
        }

        // Calculate distance between entities
        float distance = Vector2Distance(GetPosition(), otherEntities[iEntity].GetPosition());
        float combinedRadius = GetCollisionObject().radius + otherEntities[iEntity].GetCollisionObject().radius;
        
        // Check for collision
        if (distance <= combinedRadius)
        {
            printf("collision\n");
            // Calculate overlap
            float overlap = combinedRadius - distance;

            // Collision normal (direction from the other entity to this entity)
            Vector2 collisionNormal = Vector2Normalize(GetPosition() - otherEntities[iEntity].GetPosition());

            // Resolve overlap by pushing the player outward along the collision normal
            newPosition += collisionNormal * overlap/2;

            // Adjust velocity to slide along the collision surface
            // Project the velocity onto the tangent of the collision
            Vector2 collisionTangent = { -collisionNormal.y, collisionNormal.x }; // Tangent is perpendicular to the normal

            // Update adjusted velocity based on the tangent of the current collision
            adjustedVelocity = collisionTangent * Vector2DotProduct(adjustedVelocity, collisionTangent);

            // OPTIONAL: Add slight damping to prevent "jittering" near the edge
            adjustedVelocity *= 0.95f;
        }
    }

    // Update the position and velocity after processing all collisions
    _velocity = adjustedVelocity; // Slide along the collision surface
    return newPosition;
}


