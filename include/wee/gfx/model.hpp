#pragma once

#include <core/mat4.hpp>

namespace wee {

    struct index_buffer;
    struct vertex_buffer;

    struct mesh {
        vertex_buffer*  _vertices;
        index_buffer*   _indices;

    };

    struct bone {
        std::string _name;
        mat4f       _local;//_world_to_bone;
        mat4f       _bindPose;//_bindpose_inv;
        size_t      _parent;
    };
}
