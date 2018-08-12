#pragma once

#include <cmath>

namespace wee {

    struct mat4;
    struct vec3 {
        static const vec3 _one;
        static const vec3 _zero;
        static const vec3 _up;
        static const vec3 _right;
        static const vec3 _forward;

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
}
