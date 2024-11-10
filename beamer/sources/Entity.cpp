#include "Entity.h"
#include "math.h"

Entity::Entity(std::string texturePath) {
    std::string path = std::string(ASSETS_PATH) + texturePath;
    _texture = LoadTexture(path.c_str());
}

Entity::~Entity() {
    UnloadTexture(_texture);
}


void Entity::Update() 
{
    _rotation = _rotation + _rotationSpeed*GetFrameTime();
    if (_rotation > 360.0f)
    {
        _rotation = _rotation - 360.0f;
    }
}

void Entity::SetColor(Color color)
{
    _color = PINK;
}

void Entity::SetScale(float scale)
{
    _scale = scale;
}

void Entity::Draw(Camera2D &camera) {
    BeginMode2D(camera);
        Rectangle source = {0.0f, 0.0f, (float)_texture.width, (float)_texture.height};
        Rectangle target = GetRect();
        _scale = (float)N_TILE_SIZE/fmax(_texture.width, _texture.height);
        _origin = {_scale*target.width/2.0f, _scale*target.height/2.0f};
        DrawTexturePro(_texture, source, target, _origin, _rotation, _color);
    EndMode2D();
}

Vector2 Entity::GetPosition() {
    return _position;
}


Circle Entity::GetCollisionObject() {
    return Circle{_position, fmax(GetRect().width*0.5f, GetRect().height*0.5f)};
}

Rectangle Entity::GetRect()
{
    return  {_position.x, _position.y, _texture.width*_scale, _texture.height*_scale};
}

void Entity::SetPosition(Vector2 pos) 
{
    _position = pos;
}


void Entity::VelocityMove(Vector2 direction)
{

    Vector2  dv = Vector2Normalize(direction) * _acceleration * GetFrameTime();

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
        else if (_velocity.x < 0)
        {
            dv.y = _deceleration * GetFrameTime();   
        }
    }
    else
    {
        dv.y = direction.y * _acceleration * GetFrameTime();        
    }

     _velocity = _velocity + dv;

     if (Vector2Length(_velocity) > _speed) {
         _velocity = Vector2Scale(Vector2Normalize(_velocity), _speed);
     }

     _position += _velocity * GetFrameTime();
}

