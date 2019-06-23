
#pragma once

#include <core/mat4.hpp>
#include <core/vec3.hpp>
#include <core/vec2.hpp>
#include <core/quaternion.hpp>

#define MAX_SHAKE_X     5.f
#define MAX_SHAKE_Y     5.f
#define MAX_SHAKE_Z     5.f

#define DEFINE_LEGACY_FUNCTIONS \
    float get_zoom() { return 1.f; } \
    void set_zoom(float) {} \
    void set_viewport(float, float) {} \
    void update(int) {} \
    void shake(int, bool) {} 

namespace wee {

    class camera {
        quaternion _q;
        vec3f _x;
        bool _dirty;
        mat4 _transform;
    public:
        camera();
        const mat4& get_transform();
        void set_position(float, float, float);
        void set_rotation(float, float, float);
        void lookat(float, float, float);

    public:
        void move_forward(float);
        void strafe(float);
        void rotate(float, float, float);

        DEFINE_LEGACY_FUNCTIONS;
    };
}


