#pragma once

namespace wee {
    struct plane;
    struct mat4;

    void frustum(plane*, const mat4&, bool = true);
}
