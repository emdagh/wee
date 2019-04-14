#pragma once

#include <core/vec3.hpp>
#include <core/mat4.hpp>

namespace wee {

    struct mat4;

    struct aabb {
        vec3f min = vec3f::_zero, max = vec3f::_zero;
        static aabb transform(const aabb&, const mat4&);
        aabb& add(const vec3f&);
        vec3 get_corner(int) const;
    };
}
