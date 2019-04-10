#include <gfx/model.hpp>
#include <gfx/SDL_ColorEXT.hpp>

using namespace wee;


void model::draw(graphics_device* dev) {
    for(const auto& mesh: _meshes) {
        
        dev->set_vertex_buffer(mesh._vertices);
        dev->set_index_buffer(mesh._indices);

        for(const auto& p: mesh._parts) {
            const auto& part = p.second;

            dev->draw_indexed_primitives<primitive_type::kLineList, index_type::kUnsignedInt>(
                part.base_vertex, 
                part.base_index, 
                part.num_indices, 
                part.base_index
            );
        }
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
