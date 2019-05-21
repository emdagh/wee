#pragma once

#include <core/mat4.hpp>
#include <core/aabb.hpp>
#include <gfx/vertex_buffer.hpp>
#include <gfx/index_buffer.hpp>
#include <gfx/graphics_device.hpp>
#include <vector>
#include <map>

namespace wee {
    struct binary_reader;
    struct binary_writer;
    struct graphics_device;
    struct texture;
    
    struct bone {
        std::string _name;
        mat4f       _local;//_world_to_bone;
        mat4f       _bindPose;//_bindpose_inv;
        size_t      _parent;
    };


    struct model_mesh {
        int material_index;
        int base_vertex;
        int base_index;
        size_t num_vertices;
        size_t num_indices;        
    };

    struct material {
        SDL_Color Kd;
    };
    
    struct model {
        vertex_buffer* _vertices;
        index_buffer* _indices;
       // std::map<size_t, model_mesh_part> _parts;
        std::vector<model_mesh*> _meshes;
        std::vector<material*> _materials;
        aabb _aabb;

        void draw(graphics_device*);

        static model* read(binary_reader&);
        static void write(const model*, binary_writer&);
    };

    std::ostream& operator << (std::ostream& os, const material& mat);


    /*
    struct model_mesh {
        vertex_buffer*  _vertices;
        index_buffer*   _indices;
        aabb _bounds;
        int base_vertex;
        int base_index;
        size_t num_vertices;
        size_t num_indices;
    };

    struct model {
        std::vector<model_mesh> _meshes;
        void draw(graphics_device*);
    };*/
}
