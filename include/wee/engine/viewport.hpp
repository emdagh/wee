#pragma once

namespace wee {

    struct vec3;
    struct mat4;

    struct viewport {
        float depth_min, depth_max;
        int x, y, w, h;

        static void project(const vec3& in_, const mat4& wvp_, const viewport& vp_, vec3* out_);
        static void unproject(const vec3& in_, const mat4& wvp_, const viewport& vp_, vec3* out_);
    };
}
