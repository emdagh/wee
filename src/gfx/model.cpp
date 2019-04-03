#include <gfx/model.hpp>

using namespace wee;


void model::draw(graphics_device* dev) {
    for(const auto& mesh: _meshes) {
        
        dev->set_vertex_buffer(mesh._vertices);
        dev->set_index_buffer(mesh._indices);

        for(const auto& part: mesh._parts) {

            dev->draw_indexed_primitives<primitive_type::kLineList, index_type::kUnsignedInt>(
                part.base_vertex, 
                part.base_index, 
                part.num_indices, 
                part.base_index
            );
        }
    }
}
