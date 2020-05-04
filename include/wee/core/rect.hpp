#pragma once

namespace wee {
    struct rect {
        int x, y;
        int width, height;

        int right() const { return x + width; }
        int bottom() const { return y + height; }
    }
}