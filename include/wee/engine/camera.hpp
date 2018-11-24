
#pragma once

#include <core/mat4.hpp>
#include <core/random.hpp>
#include <core/vec3.hpp>
#include <core/vec2.hpp>

#define MAX_SHAKE_X     5.f
#define MAX_SHAKE_Y     5.f

namespace wee {
    class camera {
        mat4 _transform;
        float _zoom = 1.0f;;
        float _rotation = 0.0f;
        bool _changed = true;
        bool _shaking = false;
        bool _restore_after = true;
        vec2 _position = { 0.0f, 0.0f };
        vec2 _stored_position = { 0.0f, 0.0f };
        vec2 _viewport = { 0.0f, 0.0f };
        int _shaketime = 0;
    protected:
        void _update_transform();
    public:
        void set_viewport(int w, int h);
        void set_position(float x, float y);
        void shake(int t, bool restorePositionAfter = true);
        void end_shake();
        void update(int dt);
        const mat4& get_transform();
        void screen_to_world(const vec3& src, vec3* dst);
        void set_zoom(float a) { 
            _zoom = a; 
        }
        float get_zoom() const { return _zoom; }
    };
}


