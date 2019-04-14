#include <core/quaternion.hpp>
#include <core/vec3.hpp>
#include <nlohmann/json.hpp>

namespace wee {
    std::ostream& operator << (std::ostream& os, const quaternion& q)  {
        return os << nlohmann::json { 
            {"x", q.x }, 
            {"y", q.y },
            {"z", q.z },
            {"w", q.w }
        };
    }
}

using namespace wee;

quaternion quaternion::axis_angle(const vec3& axis, float angle) {
    quaternion q;
    float num2 = angle * 0.5f;
    float num = std::sin(num2);
    float num3 = std::cos(num2);
    q.x = axis.x * num;
    q.y = axis.y * num;
    q.z = axis.z * num;
    q.w = num3;
    return q;
}

vec3 quaternion::transform(const vec3& v, const quaternion& q)
{
    vec3 r;
    float x =  q.y * v.z - q.z * v.y + q.w * v.x;
    float y = -q.x * v.z + q.z * v.x + q.w * v.y;
    float z =  q.x * v.y - q.y * v.x + q.w * v.z;
    float w = -q.x * v.x - q.y * v.y - q.z * v.z;

    r.x =  x *  q.w + y * -q.z - z * -q.y + w * -q.x;
    r.y = -x * -q.z + y *  q.w + z * -q.x + w * -q.y;
    r.z =  x * -q.y - y * -q.x + z *  q.w + w * -q.z;
    return r;
}

quaternion quaternion::lookat(const vec3& sourcePoint, const vec3& destPoint, const vec3& up) {

    vec3 forwardVector = vec3::normalized(destPoint - sourcePoint);

    float dot = vec3::dot(vec3::_forward, forwardVector);

    if (std::abs(dot - (-1.0f)) < 0.000001f)
    {
        return  quaternion { up.x, up.y, up.z, M_PI };
    }
    if (std::abs(dot - (1.0f)) < 0.000001f)
    {
        return quaternion { 0.0f, 0.0f, 0.0f, 1.0f }; //Quaternion.identity;
    }

    float rotAngle = std::acos(dot);
    vec3 rotAxis = vec3::cross(vec3::_forward, forwardVector);
    rotAxis = vec3::normalized(rotAxis);
    return  quaternion::axis_angle(rotAxis, rotAngle);
}
