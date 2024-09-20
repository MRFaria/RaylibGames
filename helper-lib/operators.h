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

inline Vector2 operator/(const Vector2& v1, const Vector2& v2)
{
    return Vector2Divide(v1, v2);
}

#endif //OPERATORS_H