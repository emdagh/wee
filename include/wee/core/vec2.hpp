#pragma once

#include <iostream>
#include <cmath>
#include <wee.hpp>

namespace wee {
    template <typename T>
    struct basic_vec2 {
        union {
            struct {
                T x, y;
            };
            T cell[2];
        };

        basic_vec2 operator - () const {
            basic_vec2 copy(*this);
            copy.x = -copy.x;
            copy.y = -copy.y;
            return copy;
        }

        basic_vec2 operator + (const basic_vec2& other) const {
            basic_vec2 copy(*this);
            copy += other;
            return copy;
        }

        basic_vec2& operator += (const basic_vec2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
        basic_vec2 operator - (const basic_vec2& other) const {
            basic_vec2 copy(*this);
            copy -= other;
            return copy;
        }

        basic_vec2& operator -= (const basic_vec2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        basic_vec2 operator * (const T f) const {
            basic_vec2 copy(*this);
            copy *= f;
            return copy;
        }

        basic_vec2& operator *= (const T f) {
            x *= f;
            y *= f;
            return *this;
        }

        basic_vec2 operator / (const T f) const {
            basic_vec2 copy(*this);
            copy /= f;
            return copy;
        }

        basic_vec2& operator /= (const T f) {
            T f_r = ::one<T>() / f;
            x *= f_r;
            y *= f_r;
            return *this;
        }

        const basic_vec2& operator = (const basic_vec2& other) {
            x = other.x;
            y = other.y;
            return *this;
        }

        static basic_vec2 from_angle(float rad) {
            basic_vec2 res;
            res.x = std::cos(rad);
            res.y = std::sin(rad);
            return res;
            //return { std::cos(rad), std::sin(rad) };
        }

        static float to_angle(const basic_vec2& a) {
            return std::atan2(a.x, -a.y);
        }

        static float dot(const basic_vec2& a, const basic_vec2& b) {
            return a.x * b.x + a.y * b.y;
        }

        static float length(const basic_vec2& a) {
            return std::sqrt(dot(a, a));
        }

        static basic_vec2 normalize(const basic_vec2& a) {
            float len_r = ::one<T>() / length(a);
            return { a.x * len_r, a.y * len_r };
        }

        static basic_vec2 normal_of(const basic_vec2& a) { // normal is to the left of direction
            basic_vec2 n = normalize(a);
            return { -n.y, n.x };
        }

        static basic_vec2 rotate_at(const basic_vec2& in, const basic_vec2& at, const T& t) {
            float c = std::cos(t);
            float s = std::sin(t);

            basic_vec2 out;

            out.x = at.x + (in.x * c - in.x * s);
            out.y = at.y + (in.y * s + in.y * c);

            return out;
        }
    };

    typedef basic_vec2<float> vec2f;
    typedef basic_vec2<int32_t> vec2i;
    typedef basic_vec2<uint32_t> vec2ui;
    typedef vec2f vec2;

    template <typename T>
    std::ostream& operator << (std::ostream& os, const basic_vec2<T>& vec) {
        return os << "{" << vec.x << ", " << vec.y << "}";
    }



}
