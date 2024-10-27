#ifndef SWEPT_AABB_H
#define SWEPT_AABB_H

#include "raylib.h"
#include <array>

namespace helper
{
    struct DynamicRect {
            Rectangle rect;
            Vector2 vel;
            std::array<Rectangle, 4> contact;
    };
    struct DynamicCircle {
        Vector2 position; // Center of the circle
        float radius;     // Radius of the circle
        Vector2 velocity; // Velocity of the circle
    };
    bool RayVsRect(const Vector2& ray_origin, const Vector2& ray_dir, const Rectangle& rec, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near);
    bool DynamicRectVsRect(const DynamicRect& r_dynamic, const float fTimeStep, const Rectangle& r_static, Vector2& contact_point, Vector2& contact_normal, float& contact_time);
    bool CircleVsRect(const DynamicCircle& circle, const Rectangle& rect, Vector2& contactPoint, Vector2& contactNormal,float& contactTime, float frameTime);
}
#endif // SWEPT_AABB_H