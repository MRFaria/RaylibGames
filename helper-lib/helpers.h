#ifndef HELPERS_H
#define HELPERS_H

#include "raylib.h"
#include "raymath.h"


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

#include "raylib.h"
#include <cmath>

class Vector2Int {
public:
    int x, y;

    Vector2Int() : x(0), y(0) {}
    Vector2Int(int x, int y) : x(x), y(y) {}
    Vector2Int(Vector2 v)
    {
        x = v.x;
        y = v.y;
    }

    // Addition operator
    Vector2Int operator+(const Vector2Int& other) const {
        return Vector2Int(x + other.x, y + other.y);
    }

    // Subtraction operator
    Vector2Int operator-(const Vector2Int& other) const {
        return Vector2Int(x - other.x, y - other.y);
    }

    // Min operation
    Vector2Int min(const Vector2Int& other) const {
        return Vector2Int(x < other.x ? x : other.x, y < other.y ? y : other.y);
    }

    // Max operation
    Vector2Int max(const Vector2Int& other) const {
        return Vector2Int(x > other.x ? x : other.x, y > other.y ? y : other.y);
    }

    // Floor a Vector2 to convert to Vector2Int
    static Vector2Int floor(const Vector2& vec) {
        return Vector2Int(static_cast<int>(std::floor(vec.x)), static_cast<int>(std::floor(vec.y)));
    }

    // Convert to Vector2 for drawing
    Vector2 toVector2() const {
        return Vector2{ static_cast<float>(x), static_cast<float>(y) };
    }
};
#endif //OPERATORS_H