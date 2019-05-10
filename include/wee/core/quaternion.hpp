#pragma once

#include <numeric>
#include <limits>
#include <cmath>
#include <iosfwd>

namespace wee {
    struct vec3;
    struct quaternion {
        float x, y, z, w;

        typedef quaternion& ref;
        typedef const quaternion& const_ref;

        static const constexpr quaternion identity() {
            return quaternion { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
        }

        static quaternion axis_angle(const vec3& axis, float angle);

        static void normalize(quaternion& a) {
            float ilen = 1.0f / quaternion::length(a);
            a.x *= ilen;
            a.y *= ilen;
            a.z *= ilen;
            a.w *= ilen;
        }

        static float length(const quaternion& a) {
            return std::sqrt(dot(a, a));
        }

        static quaternion euler_angles(float yaw, float pitch, float roll) {
            float y_half = yaw * 0.5f;
            float p_half = pitch * 0.5f;
            float r_half = roll * 0.5f;

            float r_sin = std::sin(r_half);
            float r_cos = std::cos(r_half);
            float p_sin = std::sin(p_half);
            float p_cos = std::cos(p_half);
            float y_sin = std::sin(y_half);
            float y_cos = std::cos(y_half);

            return quaternion {
                ((y_cos * p_sin) * r_cos) + ((y_sin * p_cos) * r_sin),
                ((y_sin * p_cos) * r_cos) - ((y_cos * p_sin) * r_sin),
                ((y_cos * p_cos) * r_sin) - ((y_sin * p_sin) * r_cos),
                ((y_cos * p_cos) * r_cos) + ((y_sin * p_sin) * r_sin),
            };
        }

        static quaternion normalized(const quaternion& a) {
            quaternion copy(a);
            normalize(copy);
            return copy;
        }

        static quaternion lookat(const vec3& pos, const vec3& target, const vec3& up);


        static quaternion slerp(const_ref a, const_ref b, float c) { //TODO: triple check this
            if(c <= 0.0f) return a;
            if(c >= 1.0f) return b;

            quaternion q = b;
            float dp = quaternion::dot(a, b);
            if(dp < 0.0f) {
                q = -q;
                dp = -dp;
            }

            if(dp > 1.0f - std::numeric_limits<float>::epsilon()) {
                return lerp(a, q, c);
            }

            auto acos_c = std::acos(dp);
            return (a * std::sin((1.0f - c) * acos_c) + q * std::sin(c * acos_c)) / std::sin(acos_c);
        }

        static quaternion lerp(const_ref a, const_ref b, float c) {
            return quaternion {
                a.x + (b.x - a.x) * c,
                a.y + (b.y - a.y) * c,
                a.z + (b.z - a.z) * c,
                a.w + (b.w - a.w) * c,
            };
        }

        static float dot(const_ref a, const_ref b) {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        quaternion operator + (const_ref a) const {
            quaternion copy(*this);
            copy += a;
            return copy;
        }

        ref operator += (const_ref a) {
            w += a.w; x += a.x; y += a.y; z += a.z;
            return *this;
        }

        quaternion operator * (const_ref a) const {
            quaternion copy(*this);
            copy *= a;
            return copy;
        }

        ref operator *= (const_ref a) {

            auto tw = w * a.w - x * a.x - y * a.y - z * a.z;
            auto tx = w * a.x + x * a.w + z * a.y - y * a.z;
            auto ty = w * a.y + y * a.w + x * a.z - z * a.x;
            auto tz = w * a.z + z * a.w + y * a.x - x * a.y;

            x = tx;
            y = ty;
            z = tz;
            w = tw;
            return *this;
        }

        quaternion operator * (float a) const {
            quaternion copy(*this);
            copy *= a;
            return copy;
        }

        ref operator *= (float a) {
            x *= a; y *= a; z *=a; w *= a;
            return *this;
        }

        quaternion operator / (float a) const {
            quaternion copy(*this);
            copy /= a;
            return copy;
        }

        ref operator /= (float a) {
            float tmp = 1.0f / a;
            x *= tmp;
            y *= tmp;
            z *= tmp;
            w *= tmp;
            return *this;
        }

        static vec3 transform(const vec3& v, const quaternion& q);

        quaternion operator - () { // negation, not conjugation!
            quaternion copy = {
                -x, -y, -z, -w 
            };
            
            return copy;
        }
    };

    std::ostream& operator << (std::ostream&, const quaternion&);
}
