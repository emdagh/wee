#pragma once

namespace wee {

    struct vec2 {
        float x, y;

        static float dot(const vec2& a, const vec2& b) {
            return a.x * b.x + a.y * b.y;
        }

        static void cross(const vec2& a, const vec2& b, vec2* c) {
        }
    };

}
