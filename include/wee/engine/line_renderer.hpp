#pragma once

#include <gfx/vertex_buffer.hpp>
#include <gfx/vertex_declaration.hpp>
#include <vector>

struct SDL_Color;

namespace wee {
    typedef vertex<
        attributes::position,
        attributes::primary_color
    > line_vertex;

    struct graphics_device;

    class line_renderer {
        vertex_buffer* _buf;
        std::vector<line_vertex> _vertices;
        bool _valid;
        size_t _size;
    public:
        line_renderer(size_t);
        void draw(graphics_device*);
        void resize(size_t);
        void position(size_t, float, float, float);
        void color(size_t, const SDL_Color&);
        
    };
}
