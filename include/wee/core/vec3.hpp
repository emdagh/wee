#pragma once

#include <cmath>

namespace wee {

    struct mat4;
    struct vec3 {
        float x, y, z;

        static vec3 transform(const vec3&, const mat4&);

        static float length(const vec3& a) {
            return std::sqrt(dot(a, a));
        }
        static vec3 normalized(const vec3& a) {
			float r = 1.0f / length(a);
			return { 
                a.x * r, 
                a.y * r,
                a.z * r
            };

			
        }
        static vec3 cross(const vec3& a, const vec3& b) {
			return {
				a.y * b.z - b.y * a.z,
	            b.x * a.z - a.x * b.z,
    	        a.x * b.y - b.x * a.y
			};
        }
        static float dot(const vec3& a, const vec3& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }
        vec3& operator /= (const float& a) {
            auto r = 1.f / a;
            x *= r;
            y *= r;
            z *= r;
            return *this;
        }

        vec3 operator / (const float& a) const {
            vec3 copy(*this);
            copy /= a;
            return copy;
        }
    };
}
