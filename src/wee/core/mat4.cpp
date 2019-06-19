#include <core/mat4.hpp>
#include <core/vec3.hpp>
#include <core/quaternion.hpp>
#include <numeric>
#include <limits>
using namespace wee;

mat4 mat4::create_ortho_offcenter(float left, float right, float top, float bottom, float zmin, float zmax) {
    mat4 res = identity();
    res.m11 = 2.0f / (right - left);
    res.m22 = 2.0f / (top - bottom);
    res.m33 = -2.0f / (zmax - zmin);
    res.m41 = -(right + left) / (right - left);
    res.m42 = -(top + bottom) / (top - bottom);
    res.m43 = -(zmax + zmin) / (zmax - zmin);
    return res;
}

mat4 mat4::create_perspective_fov(float fov, float aspectRatio, float zmin, float zmax) {
    mat4 res = identity();
    float num = 1.0f / std::tan(fov * 0.5f);
    float num9 = num / aspectRatio;
    res.m11 = num9;
    res.m22 = num;
    res.m33 = -(zmax / (zmax - zmin));
    res.m34 = -1.0f;
    res.m43 = -((zmax * zmin) / (zmax - zmin)); 
    res.m44 = 0.0f;
    return res;
}

mat4 mat4::create_scale(const vec3& a) {
    return mat4::create_scale(a.x, a.y, a.z);
}

mat4 mat4::create_translation(const vec3& a) {
    return mat4::create_translation(a.x, a.y, a.z);
}

mat4 mat4::create_from_quaternion(const quaternion& q) {
    mat4 m;
    float num9 = q.x * q.x;
    float num8 = q.y * q.y;
    float num7 = q.z * q.z;
    float num6 = q.x * q.y;
    float num5 = q.z * q.w;
    float num4 = q.z * q.x;
    float num3 = q.y * q.w;
    float num2 = q.y * q.z;
    float num = q.x * q.w;
    m.m11 = 1.f - (2.f * (num8 + num7));
    m.m12 = 2.f * (num6 + num5);
    m.m13 = 2.f * (num4 - num3);
    m.m14 = 0.f;
    m.m21 = 2.f * (num6 - num5);
    m.m22 = 1.f - (2.f * (num7 + num9));
    m.m23 = 2.f * (num2 + num);
    m.m24 = 0.f;
    m.m31 = 2.f * (num4 + num3);
    m.m32 = 2.f * (num2 - num);
    m.m33 = 1.f - (2.f * (num8 + num9));
    m.m34 = 0.f;
    m.m41 = 0.f;
    m.m42 = 0.f;
    m.m43 = 0.f;
    m.m44 = 1.f;
    return m;
}

mat4 mat4::inverted(const mat4& in) 
{
    mat4 res = mat4::identity();
    

	float num23 = (in.m33 * in.m44) - (in.m34 * in.m43);
    float num22 = (in.m32 * in.m44) - (in.m34 * in.m42);
    float num21 = (in.m32 * in.m43) - (in.m33 * in.m42);
    float num20 = (in.m31 * in.m44) - (in.m34 * in.m41);
    float num19 = (in.m31 * in.m43) - (in.m33 * in.m41);
    float num18 = (in.m31 * in.m42) - (in.m32 * in.m41);
    float num39 = ((in.m22 * num23) - (in.m23 * num22)) + (in.m24 * num21);
    float num38 = -(((in.m21 * num23) - (in.m23 * num20)) + (in.m24 * num19));
    float num37 = ((in.m21 * num22) - (in.m22 * num20)) + (in.m24 * num18);
    float num36 = -(((in.m21 * num21) - (in.m22 * num19)) + (in.m23 * num18));
    float num = (float) 1 / ((((in.m11 * num39) + (in.m12 * num38)) + (in.m13 * num37)) + (in.m14 * num36));
    res.m11 = num39 * num;
    res.m21 = num38 * num;
    res.m31 = num37 * num;
    res.m41 = num36 * num;
    res.m12 = -(((in.m12 * num23) - (in.m13 * num22)) + (in.m14 * num21)) * num;
    res.m22 = (((in.m11 * num23) - (in.m13 * num20)) + (in.m14 * num19)) * num;
    res.m32 = -(((in.m11 * num22) - (in.m12 * num20)) + (in.m14 * num18)) * num;
    res.m42 = (((in.m11 * num21) - (in.m12 * num19)) + (in.m13 * num18)) * num;
    float num35 = (in.m23 * in.m44) - (in.m24 * in.m43);
    float num34 = (in.m22 * in.m44) - (in.m24 * in.m42);
    float num33 = (in.m22 * in.m43) - (in.m23 * in.m42);
    float num32 = (in.m21 * in.m44) - (in.m24 * in.m41);
    float num31 = (in.m21 * in.m43) - (in.m23 * in.m41);
    float num30 = (in.m21 * in.m42) - (in.m22 * in.m41);
    res.m13 = (((in.m12 * num35) - (in.m13 * num34)) + (in.m14 * num33)) * num;
    res.m23 = -(((in.m11 * num35) - (in.m13 * num32)) + (in.m14 * num31)) * num;
    res.m33 = (((in.m11 * num34) - (in.m12 * num32)) + (in.m14 * num30)) * num;
    res.m43 = -(((in.m11 * num33) - (in.m12 * num31)) + (in.m13 * num30)) * num;
    float num29 = (in.m23 * in.m34) - (in.m24 * in.m33);
    float num28 = (in.m22 * in.m34) - (in.m24 * in.m32);
    float num27 = (in.m22 * in.m33) - (in.m23 * in.m32);
    float num26 = (in.m21 * in.m34) - (in.m24 * in.m31);
    float num25 = (in.m21 * in.m33) - (in.m23 * in.m31);
    float num24 = (in.m21 * in.m32) - (in.m22 * in.m31);
    res.m14 = -(((in.m12 * num29) - (in.m13 * num28)) + (in.m14 * num27)) * num;
    res.m24 = (((in.m11 * num29) - (in.m13 * num26)) + (in.m14 * num25)) * num;
    res.m34 = -(((in.m11 * num28) - (in.m12 * num26)) + (in.m14 * num24)) * num;
    res.m44 = (((in.m11 * num27) - (in.m12 * num25)) + (in.m13 * num24)) * num;
    return res;
}

mat4 mat4::create_lookat(const vec3& eye, const vec3& at, const vec3& up) {

	vec3 vz = vec3::normalized(eye - at);

    vec3 vx = vec3::cross(up, vz);

    if (vec3::dot(vx, vx) < std::numeric_limits<float>::epsilon())
        vx = vec3::right();
    else
        vx = vec3::normalized(vx);

    vec3 vy = vec3::normalized(vec3::cross(vz, vx));

    mat4 res;
    res.m11 = vx.x;  res.m12 = vy.x;  res.m13 = vz.x; res.m14 = (float)0.;
    res.m21 = vx.y;  res.m22 = vy.y;  res.m23 = vz.y; res.m24 = (float)0.;
    res.m31 = vx.z;  res.m32 = vy.z;  res.m33 = vz.z; res.m34 = (float)0.;

    res.m41 = vec3::dot(vx, -eye);
    res.m42 = vec3::dot(vy, -eye);
    res.m43 = vec3::dot(vz, -eye);

    res.m44 = (float)1.;
    return res;
}

#include <nlohmann/json.hpp>

namespace wee {
    using nlohmann::json;

    void to_json(json& j, const mat4& m) {
        j = {
            { m.m11, m.m12, m.m13, m.m14 },
            { m.m21, m.m22, m.m23, m.m24 },
            { m.m31, m.m32, m.m33, m.m34 },
            { m.m41, m.m42, m.m43, m.m44 }
        };
    }

    void from_json(const json& j, mat4& m) {
    }

    std::ostream& operator << (std::ostream& os, const mat4& m) {
        json j;
        to_json(j, m);
        return os << j;
    }
}
