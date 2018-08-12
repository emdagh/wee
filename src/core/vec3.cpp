#pragma once

#include <core/vec3.hpp>
#include <core/mat4.hpp>
#include <iostream>

using namespace wee;

vec3 vec3::transform(const vec3&, const mat4&) {
	vec3 res;
	res.x = (a.x * b.m11) + (a.y * b.m21) + (a.z * b.m31) + b.m41;
  	res.y = (a.x * b.m12) + (a.y * b.m22) + (a.z * b.m32) + b.m42;
  	res.z = (a.x * b.m13) + (a.y * b.m23) + (a.z * b.m33) + b.m43;
}


static <typename S, typename T>
S& operator << (S& os, const vec3& self) 
{
    return os << "["<<self.x<<","<<self.y<<","<<self.z<<"]", os;
}
