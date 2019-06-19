#pragma once

#define PROPERTY(T, x, d)    public: T x(void) { return _##x; } graphics_initializer& x(T t) { _##x = t; return *this; } private: T _##x = d;

namespace wee {
    class graphics_initializer {
        PROPERTY(int, depth_bits, 24);
        PROPERTY(int, stencil_bits, 8);
        PROPERTY(int, width, 640);
        PROPERTY(int, height, 480);
        PROPERTY(bool, vsync, true);
    };
}
