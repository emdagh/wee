#pragma once

#include <core/mat4.hpp>

namespace wee {

    struct index_buffer;
    struct vertex_buffer;
    struct texture;

    struct material {
        texture* _diffuse;
    };

    struct bone {
        std::string _name;
        mat4f       _local;//_world_to_bone;
        mat4f       _bindPose;//_bindpose_inv;
        size_t      _parent;
    };

    struct mesh_part {
        int base_vertex;
        int base_index;
        size_t num_vertices;
        size_t num_indices;
    };
    
    struct mesh {
        vertex_buffer*  _vertices;
        index_buffer*   _indices;
    };

    struct model {
        std::vector<mesh*> _meshes;
    };
}
