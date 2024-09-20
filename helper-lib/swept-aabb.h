#ifndef SWEPT_AABB_H
#define SWEPT_AABB_H

#include "raylib.h"
#include "raymath.h"

namespace helper
{
    //Vector2 operator+(const Vector2& v1, const Vector2& v2);
    //Vector2 operator-(const Vector2& v1, const Vector2& v2);
    //Vector2 operator*(const Vector2& v1, const Vector2& v2);
    //Vector2 operator/(const Vector2& v1, const Vector2& v2);
    bool RayVsRect(const Vector2& ray_origin, const Vector2& ray_dir, const Rectangle& rec, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near);
}
#endif // SWEPT_AABB_H