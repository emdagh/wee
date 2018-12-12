#include <engine/camera.hpp>

using namespace wee;

/*auto randf = [] (float min, float max) -> float {
    using wee::random;
    static auto milliseconds_since_epoch =
        std::chrono::system_clock::now().time_since_epoch() / 
        std::chrono::milliseconds(1);

    static random rnd(milliseconds_since_epoch);
    return rnd.next(min, max);
};*/

void camera::_update_transform() {
    mat4 Mt, Mr, Ms, Mt2;
    Mt = mat4::create_translation(-_position.x, -_position.y, 0.0f);
    Ms = mat4::create_scale(_zoom, _zoom, 1.0f);
    Mr = mat4::create_rotation(0.f, 0.f, _rotation);
    Mt2 = mat4::create_translation(
            _viewport.x * 0.5f,
            _viewport.y * 0.5f, 
            0.0f
            );
    _transform = mat4::mul(
            mat4::mul(
                Mt,
                mat4::mul(
                    Mr, Ms
                    )
                ), Mt2
            );
    _changed = false;
}

void camera::set_viewport(int w, int h) {
    _viewport.x = (float)w;
    _viewport.y = (float)h;
    _changed = true;
}

void camera::set_position(float x, float y) {
    _position.x = x;
    _position.y = y;
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
        set_position(_stored_position.x, _stored_position.y);
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

    float px, py;
    px = _position.x + randf(-MAX_SHAKE_X, MAX_SHAKE_X) * pos;
    py = _position.y + randf(-MAX_SHAKE_Y, MAX_SHAKE_Y) * pos;
    set_position(px, py);
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
