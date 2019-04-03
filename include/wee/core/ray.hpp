#pragma once

namespace wee {
    struct plane;

    struct ray {
        vec3f origin;
        vec3f direction;

        static ray create(const plane&, const plane&);
    };
}
