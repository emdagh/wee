#include <core/aabb.hpp>
#include <core/intersect.hpp>
#include <algorithm> // MSVC specific

using namespace wee;


aabb aabb::transform(const aabb& b, const mat4& m) {
    return aabb {
        vec3::transform(b.min, m),
        vec3::transform(b.max, m)
    };
}

aabb& aabb::add(const vec3f& p) {
    min.x = std::min(p.x, min.x);
    min.y = std::min(p.y, min.y);
    min.z = std::min(p.z, min.z);

    max.x = std::max(p.x, max.x);
    max.y = std::max(p.y, max.y);
    max.z = std::max(p.z, max.z);

    return *this;
}

vec3 aabb::get_corner(int n) const {
    vec3 Out;
    Out.x = (n & 1) ? max.x : min.x;
    Out.y = (n & 2) ? max.y : min.y;
    Out.z = (n & 4) ? max.z : min.z;
    return Out;
}
