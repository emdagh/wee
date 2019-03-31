#pragma once

#include <core/mat4.hpp>
#include <core/aabb.hpp>
#include <gfx/vertex_buffer.hpp>
#include <gfx/index_buffer.hpp>
#include <gfx/graphics_device.hpp>
#include <vector>

namespace wee {

    struct graphics_device;
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
        std::vector<mesh_part> _parts;
        aabb _bounds;
    };

    struct model {
        std::vector<mesh> _meshes;
        void draw(graphics_device*);
    };
}
