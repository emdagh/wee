#pragma once

#include <cmath>
#include <iostream>

namespace wee {

    struct mat4;
	struct quaternion;
    struct vec3 {

        static const constexpr vec3 zero() {
            return vec3 { 0.0f, 0.0f, 0.0f };
        }
        static const constexpr vec3 one() {
            return vec3 { 1.0f, 1.0f, 1.0f };
        }
        static const constexpr vec3 up() {
            return vec3 { 0.0f, 1.0f, 0.0f };
        }
        static const constexpr vec3 right() {
            return vec3 { 1.0f, 0.0f, 0.0f };
        }
        static const constexpr vec3 forward() {
            return vec3 { 0.0f, 0.0f, -1.0f };
        }

        typedef const vec3& const_ref;

        float x, y, z;

        static vec3 lerp(const_ref a, const_ref b, float c) {
            vec3 res = {
                a.x + (b.x - a.x) * c,
                a.y + (b.y - a.y) * c,
                a.z + (b.z - a.z) * c
            };
            return res;
        }

        static vec3 transform(const vec3&, const mat4&);
        static vec3 transform(const vec3&, const quaternion&);

        static float length(const vec3& a) {
            return std::sqrt(dot(a, a));
        }
        static vec3 normalized(const vec3& a) {
            float len_sq = dot(a, a);
            if(len_sq == 0.0f) {
                return a;
            }
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
        inline vec3 operator - () const {
            vec3 copy(*this);
            copy.x = -copy.x;
            copy.y = -copy.y;
            copy.z = -copy.z;
            return copy;
        }

        inline vec3 operator + (const vec3& b) const {
            vec3 copy(*this);
            copy += b;
            return copy;
        }
        inline vec3 operator - (const vec3& b) const {
            vec3 copy(*this);
            copy -= b;
            return copy;
        }
        inline vec3 operator * (const vec3& b) const {
            vec3 copy(*this);
            copy *= b;
            return copy;
        }
        inline vec3 operator / (const vec3& b) const {
            vec3 copy(*this);
            copy /= b;
            return copy;
        }
        inline vec3& operator += (const vec3& b) {
            x += b.x;
            y += b.y;
            z += b.z;
            return *this;
        }
        inline vec3& operator -= (const vec3& b) {
            x -= b.x;
            y -= b.y;
            z -= b.z;
            return *this;
        }
        inline vec3& operator *= (const vec3& b) {
            x *= b.x;
            y *= b.y;
            z *= b.z;
            return *this;
        }
        inline vec3& operator /= (const vec3& b) {
            x /= b.x;
            y /= b.y;
            z /= b.z;
            return *this;
        }

        inline vec3 operator * (float a) const {
            vec3 copy(*this);
            copy *= a;
            return copy;
        }

        inline vec3& operator *= (float a) {
            x *= a;
            y *= a;
            z *= a;
            return *this;
        }

        inline vec3 operator / (float a) const {
            vec3 copy(*this);
            copy /= a;
            return copy;
        }

        inline vec3& operator /= (float a) {
            // TODO: reciprocal?
            x /= a;
            y /= a;
            z /= a;
            return *this;
        }

    };

    typedef vec3 vec3f;

    std::ostream& operator << (std::ostream&, const vec3&);
}
