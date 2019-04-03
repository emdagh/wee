
#pragma once

#include <core/mat4.hpp>
#include <core/random.hpp>
#include <core/vec3.hpp>
#include <core/vec2.hpp>
#include <core/quaternion.hpp>

#define MAX_SHAKE_X     5.f
#define MAX_SHAKE_Y     5.f
#define MAX_SHAKE_Z     5.f

namespace wee {

    class camera {
        //quaternion _orientation[2];
        vec3 _rotation;
        vec3 _position[2];
        mat4 _transform;
        bool _valid = false;

    public:
        //void lookat(const vec3f&);
        //void strafe(float);
        //void advance(float);
        void shake(int, bool = true) {}

        void transform(mat4*);
        mat4 get_transform() {
            mat4 res;
            transform(&res);
            return res;
        }
        void set_viewport(float, float) {}
        void set_position(float x, float y, float z) { _position[0] = _position[1] = vec3 { x, y, z }; _valid = false; }
        void set_rotation(float x, float y, float z) { _rotation = vec3 {x, y, z}; _valid = false; }
    public:
        void set_zoom(float) {}
        float get_zoom() { return 1.0f; }
        void update(int) {}
    };
}


