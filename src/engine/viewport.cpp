
#include <engine/viewport.hpp>
#include <core/vec3.hpp>
#include <core/mat4.hpp>
#include <util/numeric.hpp>

using namespace wee;

void viewport::project(const vec3& in_, const mat4& wvp_, const viewport& vp, vec3* out_) {
	vec3 vector = vec3::transform(in_, wvp_);
    float a = (((in_.x * wvp_.m14) + (in_.y * wvp_.m24)) + (in_.z * wvp_.m34)) + wvp_.m44;
    if (!within_epsilon(a - 1.f))
    {
        vector = (vector / a);
    }
    out_->x = ((( vector.x + 1.f) * 0.5f) * vp.w) + vp.x;
    out_->y = (((-vector.y + 1.f) * 0.5f) * vp.h) + vp.y;
    out_->z = (   vector.z * (vp.depth_max - vp.depth_min)) + vp.depth_min;
}

void viewport::unproject(const vec3& in_, const mat4& wvp_, const viewport& vp, vec3* out_) {
	mat4 wvpI = mat4::inverted(wvp_);
    vec3 vec;
    vec.x =  ((( in_.x - vp.x) / ((float) vp.w)) * 2.f) - 1.f;
    vec.y = -((((in_.y - vp.y) / ((float) vp.h)) * 2.f) - 1.f);
    vec.z =  (   in_.z - vp.depth_min) / (vp.depth_max - vp.depth_min);
    vec3 vector = vec3::transform(vec, wvpI);
    float a = (((vec.x * wvpI.m14) + (vec.y * wvpI.m24)) + (vec.z * wvpI.m34)) + wvpI.m44;

    if (!within_epsilon(a - 1.f))
    {
        *out_ = (vector / a);
        return;
    }
    *out_ = vector;
}
