#ifndef OPERATORS_H
#define OPERATORS_H

#include "raylib.h"
#include "raymath.h"

inline Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
    return Vector2Add(v1, v2);
}

inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
    return Vector2Subtract(v1, v2);
}

inline Vector2 operator*(const Vector2& v1, const Vector2& v2)
{
    return Vector2Multiply(v1, v2);
}

inline Vector2 operator*(const Vector2& v1, const float f)
{
    return Vector2{v1.x*f, v1.y*f};
}

inline Vector2 operator/(const Vector2& v1, const Vector2& v2)
{
    return Vector2Divide(v1, v2);
}

inline Vector2 operator/(const Vector2& v1, const float f)
{
    return Vector2{v1.x/f, v1.y/f};
}

inline Vector2 GetSize(Rectangle rect)
{
    return Vector2{rect.width, rect.height}; // Correctly returns width and height
}

inline Vector2 GetPos(Rectangle rect)
{
    return Vector2{rect.x, rect.y}; // Correctly returns the position of the rectangle
}

inline void SetPos(Rectangle& rect, Vector2 pos)
{
    rect.x = pos.x; // Sets the x position correctly
    rect.y = pos.y; // Sets the y position correctly
}

inline void SetSize(Rectangle& rect, Vector2 size)
{
    rect.width = size.x;   // Sets the width correctly
    rect.height = size.y;  // Sets the height correctly
}

#endif //OPERATORS_H