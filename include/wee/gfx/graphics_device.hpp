#pragma once

#include <SDL.h>
#include <weegl.h>
#include <gfx/primitive_type.hpp>
#include <gfx/vertex_buffer.hpp>
#include <gfx/index_buffer.hpp>
#include <gfx/vertex_declaration.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/draw.hpp>

#include <core/enum_cast.hpp>
#include <core/range.hpp>
#include <cassert>
#include <stack>



//struct SDL_Renderer;

/*#define DECLARE_PRIMITIVE(a, b) \
    template <> \
    struct glGetPrimitiveType<primitive_type::a> { \
        static const GLenum type = b;\
    }

    */

namespace wee {
    /*
    template <primitive_type T>
    struct glGetPrimitiveType {
        static const GLenum type = GL_NONE;
    };

    DECLARE_PRIMITIVE(line_list,    GL_LINES);
    DECLARE_PRIMITIVE(line_strip,   GL_LINE_STRIP);
    DECLARE_PRIMITIVE(line_loop,    GL_LINE_LOOP);
    DECLARE_PRIMITIVE(points,      GL_POINTS);
    DECLARE_PRIMITIVE(triangles,   GL_TRIANGLES);
    DECLARE_PRIMITIVE(triangle_fan, GL_TRIANGLE_FAN);
    DECLARE_PRIMITIVE(triangle_strip, GL_TRIANGLE_STRIP);
    DECLARE_PRIMITIVE(quads,       GL_QUADS);
    DECLARE_PRIMITIVE(quad_strip,   GL_QUAD_STRIP);
    */
    /*template <>
    struct glGetPrimitiveType<primitive_type::kTriangles> {
        static const GLenum type =  GL_TRIANGLES;
    };
    template <>
    struct glGetPrimitiveType<primitive_type::kLineList> {
        static const GLenum type =  GL_LINE_STRIP;
    };*/

    /*
    template <index_type T>
    struct glGetIndexType {
        static const GLenum type =  GL_NONE;
    };

    template <>
    struct glGetIndexType<index_type::unsigned_byte> {
        static const GLenum type = GL_UNSIGNED_BYTE;
    };
    template <>
    struct glGetIndexType<index_type::unsigned_short> {
        static const GLenum type = GL_UNSIGNED_SHORT;
    };
    template <>
    struct glGetIndexType<index_type::unsigned_int> {
        static const GLenum type = GL_UNSIGNED_INT;
    };*/

    
    enum struct clear_options : uint8_t {
        kClearColor     = 0b0001,
        kClearDepth     = 0b0010,
        kClearStencil   = 0b0100,
        kClearAll       = 0b0111
    };

    struct shader_program;

	template <typename T>
	struct array_object {
		inline static GLuint vao = 0;
	};

    class graphics_device {
		GLuint _vao = 0;

        std::stack<vertex_buffer*> _vbo;
        std::stack<index_buffer*> _ibo;
        std::stack<shader_program*> _shaders;

    public:
        explicit graphics_device(SDL_Renderer* renderer);
        virtual ~graphics_device();

        virtual void clear(const SDL_Color& color, const clear_options& copt = clear_options::kClearAll, float depth = 1.0f, int stencil = 0x0) {
            constexpr static float val = 1.0f / 255;
            glClearDepthf(depth);
            glClearStencil(stencil);
            glClearColor(color.r * val, color.g * val, color.b * val, color.a * val);

            GLenum glClearOptions= GL_NONE;
            glClearOptions |= static_cast<GLenum>(copt) & static_cast<GLenum>(clear_options::kClearColor) ? GL_COLOR_BUFFER_BIT : GL_NONE;
            glClearOptions |= static_cast<GLenum>(copt) & static_cast<GLenum>(clear_options::kClearDepth) ? GL_DEPTH_BUFFER_BIT : GL_NONE;
            glClearOptions |= static_cast<GLenum>(copt) & static_cast<GLenum>(clear_options::kClearStencil) ? GL_STENCIL_BUFFER_BIT : GL_NONE;
            glClear(glClearOptions);
        }
        virtual void set_rendertarget(SDL_Texture* t) {
            //SDL_SetRenderTarget(_renderer, t);
        }

		void set_index_buffer(index_buffer* buf);

		template<typename T>
		void set_vertex_buffer(vertex_buffer* buf) {
			if (_vao == 0) //array_object<T>::vao == 0) 
				glGenVertexArrays(1, &_vao);//&array_object<T>::vao);

			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, buf->_handle);
			install_vertex_attributes<T, vertex_attribute_installer>();
		}


        template <primitive_type P>
        void draw_primitives(int first, size_t count, size_t primitiveCount) {
			assert(_vao != 0);
			//glBindVertexArray(_vao);
#if 0
            for(auto i: range(primitiveCount)) {
                glDrawArrays(glGetPrimitiveType<P>::type //<enum_cast(p)>::type
                        , start + count * i, count);
            }
#else
			glDrawArrays(glGetPrimitiveType<P>::type, first, count);
#endif
			//glBindVertexArray(0);
        }

        template <primitive_type P, index_type I>
        void draw_indexed_primitives(
            int baseVertex, 
            int minIndex, 
            size_t count, 
            int indexOffset
			)
        {
			assert(_vao != 0);
			//glBindVertexArray(_vao);
#if 1 
			glDrawElementsBaseVertex(
				glGetPrimitiveType<P>::type,
				count,
				glGetIndexType<I>::type,
				BUFFER_OFFSET(0),
				baseVertex
			);
#else
            glDrawElements(glGetPrimitiveType<P>::type,
                count,
                glGetIndexType<I>::type,
                BUFFER_OFFSET(minIndex)
            );
#endif
			
			//glBindVertexArray(0);
        }
    };
}

#if defined(__cpluscplus)
using wee_graphics_device = wee::graphics_device;
extern "C" {
#else
struct wee_graphics_device;
#endif
int create_graphics_device(wee_graphics_device**);
void create_graphics_device(wee_graphics_device*);
#if defined(__cpluscplus)
}
#endif
