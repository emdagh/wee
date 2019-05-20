#include <gfx/model.hpp>
#include <gfx/SDL_ColorEXT.hpp>

using namespace wee;


void model::draw(graphics_device* dev) {
    dev->set_vertex_buffer(_vertices);
    dev->set_index_buffer(_indices);
    for(const auto& mesh: _meshes) {
        //for(const auto& p: mesh._parts) {

            dev->draw_indexed_primitives<primitive_type::line_list, index_type::unsigned_int>(
                mesh.base_vertex, 
                mesh.base_index, 
                mesh.num_indices, 
                mesh.base_index
            );
        //}
    }
}
namespace wee {
    std::ostream& operator << (std::ostream& os, const SDL_Color& c) {
        return os << static_cast<int>(c.r) << "," << static_cast<int>(c.g) << "," << static_cast<int>(c.b) << "," << static_cast<int>(c.a);
    }
    std::ostream& operator << (std::ostream& os, const material& mat) {
        return os << mat.Kd;
    }
}
