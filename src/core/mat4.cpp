#include <core/mat4.hpp>

using namespace wee;

mat4 mat4::inverted(const mat4& in) 
{
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
    out->m11 = num39 * num;
    out->m21 = num38 * num;
    out->m31 = num37 * num;
    out->m41 = num36 * num;
    out->m12 = -(((in.m12 * num23) - (in.m13 * num22)) + (in.m14 * num21)) * num;
    out->m22 = (((in.m11 * num23) - (in.m13 * num20)) + (in.m14 * num19)) * num;
    out->m32 = -(((in.m11 * num22) - (in.m12 * num20)) + (in.m14 * num18)) * num;
    out->m42 = (((in.m11 * num21) - (in.m12 * num19)) + (in.m13 * num18)) * num;
    float num35 = (in.m23 * in.m44) - (in.m24 * in.m43);
    float num34 = (in.m22 * in.m44) - (in.m24 * in.m42);
    float num33 = (in.m22 * in.m43) - (in.m23 * in.m42);
    float num32 = (in.m21 * in.m44) - (in.m24 * in.m41);
    float num31 = (in.m21 * in.m43) - (in.m23 * in.m41);
    float num30 = (in.m21 * in.m42) - (in.m22 * in.m41);
    out->m13 = (((in.m12 * num35) - (in.m13 * num34)) + (in.m14 * num33)) * num;
    out->m23 = -(((in.m11 * num35) - (in.m13 * num32)) + (in.m14 * num31)) * num;
    out->m33 = (((in.m11 * num34) - (in.m12 * num32)) + (in.m14 * num30)) * num;
    out->m43 = -(((in.m11 * num33) - (in.m12 * num31)) + (in.m13 * num30)) * num;
    float num29 = (in.m23 * in.m34) - (in.m24 * in.m33);
    float num28 = (in.m22 * in.m34) - (in.m24 * in.m32);
    float num27 = (in.m22 * in.m33) - (in.m23 * in.m32);
    float num26 = (in.m21 * in.m34) - (in.m24 * in.m31);
    float num25 = (in.m21 * in.m33) - (in.m23 * in.m31);
    float num24 = (in.m21 * in.m32) - (in.m22 * in.m31);
    out->m14 = -(((in.m12 * num29) - (in.m13 * num28)) + (in.m14 * num27)) * num;
    out->m24 = (((in.m11 * num29) - (in.m13 * num26)) + (in.m14 * num25)) * num;
    out->m34 = -(((in.m11 * num28) - (in.m12 * num26)) + (in.m14 * num24)) * num;
    out->m44 = (((in.m11 * num27) - (in.m12 * num25)) + (in.m13 * num24)) * num;
}

mat4 mat4::create_lookat(const vec3& eye, const vec3& at, const vec3& up) {
	vec3 vz = vec3::normalized(eye - at);

    vec3 vx = vec3::cross(up, vz);

    if (vec3::dot(vx, vx) < std::numeric_limits<float>::epsilon())
        vx = vec3::Right;
    else
        vx = vec3::normalized(vx);

    vec3 vy = vec3::normalized(vec3::cross(vz, vx));


    out->m11 = vx.X;  out->m12 = vy.X;  out->m13 = vz.X; out->m14 = (float)0.;
    out->m21 = vx.Y;  out->m22 = vy.Y;  out->m23 = vz.Y; out->m24 = (float)0.;
    out->m31 = vx.Z;  out->m32 = vy.Z;  out->m33 = vz.Z; out->m34 = (float)0.;

    out->m41 = vec3::dot(vx, -eye);
    out->m42 = vec3::dot(vy, -eye);
    out->m43 = vec3::dot(vz, -eye);

    out->m44 = (float)1.;
}
