#include <gfx/model.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <nlohmann/json.hpp>

using namespace wee;



void model::draw(graphics_device* dev) {
    dev->set_vertex_buffer(_vertices);
    dev->set_index_buffer(_indices);
    for(const auto* mesh: _meshes) {
        //for(const auto& p: mesh._parts) {

            dev->draw_indexed_primitives<primitive_type::triangles, index_type::unsigned_int>(
                mesh->base_vertex, 
                mesh->base_index, 
                mesh->num_indices, 
                mesh->base_index
            );
        //}
    }
}
model* read(binary_reader& reader) {
    return nullptr;
}
void model::write(const model* m, binary_writer& writer) {

}
namespace wee {

    using nlohmann::json;

    void to_json(json* j, const model* m) {
    }
    void from_json(const json* j, model* m) {
    }

    std::ostream& operator << (std::ostream& os, const SDL_Color& c) {
        return os << static_cast<int>(c.r) << "," << static_cast<int>(c.g) << "," << static_cast<int>(c.b) << "," << static_cast<int>(c.a);
    }
    std::ostream& operator << (std::ostream& os, const material& mat) {
        return os << mat.Kd;
    }
}
