#pragma once

#include <core/vec3.hpp>

namespace wee {
    struct plane {
        union {
            float a, b, c, d;
            struct {
                vec3f normal;
                float w;
            };
        };

        static plane normalized(const plane&);
    };
}
