#include <engine/camera.hpp>
#include <core/logstream.hpp>

using namespace wee;

camera::camera() 
: _q(quaternion::identity())
, _x(vec3::zero())
, _dirty(true)
, _transform(mat4::identity())
{
}

const mat4& camera::get_transform() {
    if(_dirty) {
        vec3 direction = vec3::transform(vec3::forward(), _q);
        _transform = mat4::create_lookat(_x, _x + direction, vec3::up());
        _dirty = false;
    }
    return _transform;
}
void camera::set_position(float x, float y, float z) {
    _x.x = x;
    _x.y = y;
    _x.z = z;
    _dirty = true;
}
void camera::set_rotation(float y, float p, float r) {
    _q = quaternion::euler_angles(y, p, r);
    _dirty = true;
}
void camera::lookat(float x, float y, float z) {
	//_q = quaternion::look_rotation(vec3 { x, y, z }, vec3::_up);
    //
    _q = quaternion::lookat(_x, vec3 { x, y, z }, vec3::up());
	_dirty = true;
}

void camera::move_forward(float x) {
    vec3 f = vec3::transform(vec3::forward(), _q);
    _x = _x + f * x;
    _dirty = true;
}
void camera::strafe(float x) {
    vec3 right = vec3::transform(vec3::right(), _q);
    _x = _x + right * x;
    _dirty = true;
}

void camera::rotate(float x, float y, float z) {
    quaternion b = quaternion::euler_angles(x, y, z);
    _q = _q * b; // multiplication is addition
    DEBUG_VALUE_OF(_q);
    _dirty = true;
}
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
