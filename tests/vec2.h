#pragma once

#include <iostream>

struct vec2 {
    float x, y;

    vec2 operator + (const vec2& other) const {
        vec2 copy(*this);
        copy += other;
        return copy;
    }

    vec2& operator += (const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec2 operator - (const vec2& other) const {
        vec2 copy(*this);
        copy -= other;
        return copy;
    }

    vec2& operator -= (const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vec2 operator / (const float f) const {
        vec2 copy(*this);
        copy /= f;
        return copy;
    }

    vec2& operator /= (const float f) {
        float f_r = 1.0f / f;
        x *= f_r;
        y *= f_r;
        return *this;
    }

    static vec2 from_angle(float rad) {
        return { std::cos(rad), std::sin(rad) };
    }

    static float to_angle(const vec2& a) {
        return std::atan2(a.x, -a.y);
    }

    static float dot(const vec2& a, const vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    static float length(const vec2& a) {
        return std::sqrt(dot(a, a));
    }

    static vec2 normalize(const vec2& a) {
        float len_r = 1.0f / length(a);
        return { a.x * len_r, a.y * len_r };
    }

    static vec2 normal_of(const vec2& a) { // normal is to the left of direction
        vec2 n = normalize(a);
        return { -n.y, n.x };
    }
};

std::ostream& operator << (std::ostream& os, const vec2& vec) {
    return os << vec.x << ", " << vec.y;
}

std::ostream& operator << (std::ostream& os, const b2Transform& tx) {
    return os << tx.p.x << ", " << tx.p.y;
}
