
#include <core/vec3.hpp>
#include <core/mat4.hpp>
#include <core/quaternion.hpp>

namespace wee {

std::ostream& operator << (std::ostream& os, const vec3& self) {
    return os << "["<<self.x<<","<<self.y<<","<<self.z<<"]";
}
}

using namespace wee;
        
const vec3 vec3::_one = { 1.f, 1.f, 1.f};
const vec3 vec3::_zero = { .0f, .0f, .0f };
const vec3 vec3::_up = { 0.f, 1.f, 0.f };
const vec3 vec3::_right = { 1.f, 0.f, 0.f };
const vec3 vec3::_forward { 0.f, 0.f, -1.f };

vec3 vec3::transform(const vec3& a, const mat4& b) {
	vec3 res;
	res.x = (a.x * b.m11) + (a.y * b.m21) + (a.z * b.m31) + b.m41;
  	res.y = (a.x * b.m12) + (a.y * b.m22) + (a.z * b.m32) + b.m42;
  	res.z = (a.x * b.m13) + (a.y * b.m23) + (a.z * b.m33) + b.m43;
	return res;
}

vec3 vec3::transform(const vec3& v, const quaternion& q) {
	float X =  q.y * v.z - q.z * v.y + q.w * v.x;
	float Y = -q.x * v.z + q.z * v.x + q.w * v.y;
	float Z =  q.x * v.y - q.y * v.x + q.w * v.z;
	float W = -q.x * v.x - q.y * v.y - q.z * v.z;

	return vec3 {
    	X *  q.w + Y * -q.z - Z * -q.y + W * -q.x,
       -X * -q.z + Y *  q.w + Z * -q.x + W * -q.y,
    	X * -q.y - Y * -q.x + Z *  q.w + W * -q.z,
	};
}

