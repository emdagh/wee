#include <engine/line_renderer.hpp>
#include <gfx/graphics_device.hpp>
#include <ostream>

using namespace wee;

line_renderer::line_renderer(size_t n) {
    _vertices.resize(n);
    _buf = new vertex_buffer(n * sizeof(line_vertex));
    _valid = true;
    _size = 0;
}

void line_renderer::draw(graphics_device* dev) {
    if(!_valid) {
        std::ostream os(_buf);
        os.write(
            reinterpret_cast<char*>(&_vertices[0]), 
            sizeof(line_vertex) * _size 
        );
        _valid = true;
    }
    install_vertex_attributes<line_vertex, vertex_attribute_installer>();
    dev->draw_primitives<primitive_type::line_list>(0, _size, 1); 
}

void line_renderer::resize(size_t) {
    throw not_implemented();
}

void line_renderer::position(size_t ix, float x, float y, float z) {
    _vertices[ix]._position.x = x;
    _vertices[ix]._position.y = y;
    _vertices[ix]._position.z = z;
    _size = ix > _size ? ix : _size;
    _valid = false;
}

void line_renderer::color(size_t ix, const SDL_Color& c) {
    _vertices[ix]._color = static_cast<int>(c.r) << 24 | static_cast<int>(c.g) << 16 | static_cast<int>(c.b) << 8 | static_cast<int>(c.a); // could be other way around....
    _size = ix > _size ? ix : _size;
    _valid = false;
}
