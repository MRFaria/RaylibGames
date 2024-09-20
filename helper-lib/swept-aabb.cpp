
#include "swept-aabb.h"
#include "operators.h"

// Vector2 helper::operator+(const Vector2& v1, const Vector2& v2)
// {
//     return Vector2Add(v1, v2);
// }

// Vector2 helper::operator-(const Vector2& v1, const Vector2& v2)
// {
//     return Vector2Subtract(v1, v2);
// }

// Vector2 helper::operator*(const Vector2& v1, const Vector2& v2)
// {
//     return Vector2Multiply(v1, v2);
// }

// Vector2 helper::operator/(const Vector2& v1, const Vector2& v2)
// {
//     return Vector2Divide(v1, v2);
// }

bool helper::RayVsRect(const Vector2& ray_origin, const Vector2& ray_dir, const Rectangle& rec, Vector2& contact_point, Vector2& contact_normal, 
    float& t_hit_near)
{
    auto rec_pos = Vector2{rec.x, rec.y};
    auto rec_size = Vector2{rec.width, rec.height};
    contact_normal = { 0,0 };
    contact_point = { 0,0 };
    // Cache division
    Vector2 invdir = Vector2{1.0f,1.0f} / ray_dir;
    auto t_near = (rec_pos - ray_origin) / ray_dir; 
    auto t_far = (rec_pos + rec_size - ray_origin) / ray_dir;

    // sort the points, near should be smaller 
    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

    // Early rejection, no collision
    if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

    // Basically at what fraction of the ray is the near collision?
    t_hit_near = std::max(t_near.x, t_near.y);

    // Basically at what fraction of the ray is the far collision?
    float t_hit_far = std::min(t_far.x, t_far.y);

    // Reject if ray direction is pointing away from object
    if (t_hit_far < 0)
        return false;

    // Contact point of collision from parametric line equation
    contact_point = ray_origin + Vector2{t_hit_near, t_hit_near} * ray_dir;

    if (t_near.x > t_near.y)
        if (invdir.x < 0)
            contact_normal = { 1, 0 };
        else
            contact_normal = { -1, 0 };
    else if (t_near.x < t_near.y)
        if (invdir.y < 0)
            contact_normal = { 0, 1 };
        else
            contact_normal = { 0, -1 };

    // Note if t_near == t_far, collision is principly in a diagonal
    // so pointless to resolve. By returning a CN={0,0} even though its
    // considered a hit, the resolver wont change anything.
    return true;
}


