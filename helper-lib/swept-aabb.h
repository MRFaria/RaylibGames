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
    bool RayVsRect(const Vector2& ray_origin, const Vector2& ray_dir, const Rectangle& rec, Vector2& contact_point, Vector2& contact_normal, float& t_hit_near);
    bool DynamicRectVsRect(const helper::DynamicRect& r_dynamic, const float fTimeStep, const Rectangle& r_static, Vector2& contact_point, Vector2& contact_normal, float& contact_time);
}
#endif // SWEPT_AABB_H