#ifndef ENTITY_H
#define ENTITY_H

#include "globals.h"
#include "raylib.h"
#include "Level.h"
#include <string>
#include "helpers.h"

class Entity {
public:
    Entity(std::string texturePath);  // Constructor
    ~Entity();  // Constructor
    void Update();
    void Draw(Camera2D &camera);
    void SetColor(Color color);
    void SetScale(float _scale);
    Vector2 GetPosition();
    Circle GetCollisionObject();
    Rectangle GetRect();
    void SetPosition(Vector2 pos);

protected:
    Texture2D _texture;
    Circle _collisionObject;
    float _speed = 600.0f;
    float _stopThreshold = 10.0f;
    float _acceleration = 2000.0f;
    float _deceleration = 1500.0f;
    Vector2 _velocity = {0.0f, 0.0f};
    Vector2 _position = {0.0f, 0.0f};
    Vector2 _origin = {0.0f, 0.0f};
    Color _color = WHITE;      // Color of the player
    float _scale = 1.0f;
    float _rotation = 0.0f;
    float _rotationSpeed = 0.0f;

    void VelocityMove(Vector2 direction);
};

#endif // ENTITY_H
