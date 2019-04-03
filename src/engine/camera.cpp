#include <engine/camera.hpp>
#include <core/logstream.hpp>

using namespace wee;

/**
 * assuming world-space coordinates
 */
/*
void camera::lookat(const vec3f& target) {
    _orientation[0] = quaternion::look_rotation(_position[0], target);
    _valid = false;
}
void camera::advance(float) {
    _valid = false;
}
void camera::strafe(float) {
    _valid = false;
}*/


void camera::transform(mat4* p) { 
    if(!_valid) {
        //DEBUG_VALUE_OF(_rotation);
        mat4 t = mat4::create_translation(-_position[0]);
        mat4 r = mat4::create_rotation(_rotation.x, _rotation.y, _rotation.z);
        _transform = mat4::mul(t, r);
    }
    *p = _transform;
}
/*
void camera::_update_transform() {
    //mat4 Mt, Mr, Ms, Mt2;
    //Mt = mat4::create_translation(-_position.x, -_position.y, 0.0f);
    //Ms = mat4::create_scale(_zoom, _zoom, 1.0f);
    //Mr = mat4::create_rotation(0.f, 0.f, _rotation);
    Mt2 = mat4::create_translation(
            _viewport.x * 0.5f,
            _viewport.y * 0.5f, 
            0.0f
            );
    _transform = mat4::mul(mat4::mul(Mt, mat4::mul(Mr, Ms)), Mt2);

    _changed = false;
}

void camera::set_viewport(int w, int h) {
    _viewport.x = (float)w;
    _viewport.y = (float)h;
    _changed = true;
}

void camera::set_position(float x, float y, float z) {
    _position.x = x;
    _position.y = y;
    _position.z = z;
    _changed = true;

}

void camera::shake(int t, bool restorePositionAfter) {
    if(_shaking) 
        return;

    _shaking = t > 0;
    _shaketime = t;
    _restore_after = restorePositionAfter;
    _stored_position = _position;
}

void camera::end_shake() {
    _shaketime = 0;
    _shaking = false;
    if(_restore_after) 
        set_position(_stored_position.x, _stored_position.y, _stored_position.z);
}

void camera::update(int dt) {

    if(!_shaking) return;
    _shaketime -= dt;
    if(_shaketime <= 0)
        return end_shake();

    int pos = 1;
    if(randf(0.f,10.f) >= 5.f) {
        pos -= 1;
    }

    float px, py, pz;
    px = _position.x + randf(-MAX_SHAKE_X, MAX_SHAKE_X) * pos;
    py = _position.y + randf(-MAX_SHAKE_Y, MAX_SHAKE_Y) * pos;
    pz = _position.z + randf(-MAX_SHAKE_Z, MAX_SHAKE_Z) * pos;
    set_position(px, py, pz);
}

const mat4& camera::get_transform() {
    if(_changed) 
        _update_transform();
    return _transform;
}

void camera::screen_to_world(const vec3& src, vec3* dst) {
    mat4 T, I;
    T = get_transform();
    I = mat4::inverted(T);
    vec3 Pa = { src.x, src.y, 0.0f }; // 0.0 for z-axis, flatten vector.
    Pa = vec3::transform(Pa, I);
    dst->x = Pa.x;
    dst->y = Pa.y;
    dst->z = Pa.z;
}
*/
