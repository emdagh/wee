#pragma once

namespace wee {
    struct quaternion {
        float x, y, z, w;

        typedef quaternion& ref;
        typedef const quaternion& const_ref;

        static quaternion axis_angle(const vec3& axis, float angle) {
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

        static quaternion lookat(const vec3f& pos, const vec3f& target, const vec3f& up);

        static quaternion look_rotation(const vec3& a, const vec3& b) {
            vec3 axis = vec3::normalized(vec3::cross(b, a));
            float angle = std::acos(vec3::dot(a, b));
            return normalized(axis_angle(axis, angle));
        }

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

        static vec3 transform(const vec3& v, const quaternion& q)
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


        quaternion operator - () { // negation, not conjugation!
            quaternion copy = {
                -x, -y, -z, -w 
            };
            
            return copy;
        }
    };
}
