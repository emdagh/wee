#pragma once

namespace wee {

    enum struct primitive_type : uint8_t {
        points,
        triangles,
        triangle_fan,
        triangle_strip,
        quads,
        quad_strip,
        line_list,
        line_strip,
        line_loop
    };
}
