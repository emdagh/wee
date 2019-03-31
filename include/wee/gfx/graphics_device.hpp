#pragma once


#include <weegl.h>
#include <gfx/primitive_type.hpp>
#include <gfx/vertex_buffer.hpp>
#include <gfx/index_buffer.hpp>
#include <gfx/vertex_declaration.hpp>

#include <core/enum_cast.hpp>
#include <core/range.hpp>

#include <SDL.h>

//struct SDL_Renderer;

namespace wee {

    template <primitive_type T>
    struct glGetPrimitiveType {
        static const GLenum type = GL_NONE;
    };

    template <>
    struct glGetPrimitiveType<primitive_type::kTriangles> {
        static const GLenum type =  GL_TRIANGLES;
    };
    template <>
    struct glGetPrimitiveType<primitive_type::kLineList> {
        static const GLenum type =  GL_LINE_STRIP;
    };


    template <index_type T>
    struct glGetIndexType {
        static const GLenum type =  GL_NONE;
    };

    template <>
    struct glGetIndexType<index_type::kUnsignedByte> {
        static const GLenum type = GL_UNSIGNED_BYTE;
    };
    template <>
    struct glGetIndexType<index_type::kUnsignedShort> {
        static const GLenum type = GL_UNSIGNED_SHORT;
    };
    template <>
    struct glGetIndexType<index_type::kUnsignedInt> {
        static const GLenum type = GL_UNSIGNED_INT;
    };

    
    enum struct clear_options : uint8_t {
        kClearColor     = 0b0001,
        kClearDepth     = 0b0010,
        kClearStencil   = 0b0100
    };
    struct graphics_device {
        SDL_Renderer* _renderer;


        SDL_Renderer* get_renderer() const { return _renderer; }

        explicit graphics_device(SDL_Renderer* renderer) 
        : _renderer(renderer) {

        }

        virtual void clear(const SDL_Color* color, const clear_options& copt, float depth, int stencil) {
            constexpr static float val = 1.0f / 255;
            glClearDepthf(depth);
            glClearStencil(stencil);
            glClearColor(color->r * val, color->g * val, color->b * val, color->a * val);

            GLenum glClearOptions= GL_NONE;
            glClearOptions |= static_cast<GLenum>(copt) & static_cast<GLenum>(clear_options::kClearColor) ? GL_COLOR_BUFFER_BIT : GL_NONE;
            glClearOptions |= static_cast<GLenum>(copt) & static_cast<GLenum>(clear_options::kClearDepth) ? GL_DEPTH_BUFFER_BIT : GL_NONE;
            glClearOptions |= static_cast<GLenum>(copt) & static_cast<GLenum>(clear_options::kClearStencil) ? GL_STENCIL_BUFFER_BIT : GL_NONE;
            glClear(glClearOptions);
        }
        virtual void set_rendertarget(SDL_Texture* t) {
            SDL_SetRenderTarget(_renderer, t);
        }

        void set_vertex_buffer(vertex_buffer* buf) {
            glBindBuffer(GL_ARRAY_BUFFER, buf->_handle);
        }

        void set_index_buffer(index_buffer* buf) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->_handle);
        }

        template <primitive_type P>
        void draw_primitives(int start, size_t count, size_t primitiveCount) {
            for(auto i: range(primitiveCount)) {
                glDrawArrays(glGetPrimitiveType<P>::type //<enum_cast(p)>::type
                        , start + count * i, count);
            }
        }

        template <primitive_type P, index_type I>
        void draw_indexed_primitives(
            int baseVertex, 
            int minIndex, 
            size_t count, 
            int indexOffset)
        {
            glDrawElements(glGetPrimitiveType<P>::type,
                count,
                glGetIndexType<I>::type,
                BUFFER_OFFSET(minIndex)
            );
        }
    };
}
