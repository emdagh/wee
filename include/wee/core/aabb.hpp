#pragma once

#include <core/vec3.hpp>
#include <core/mat4.hpp>

namespace wee {

    struct mat4;

    struct aabb {
        vec3f min, max;
        static aabb transform(const aabb&, const mat4&);
    };
}
