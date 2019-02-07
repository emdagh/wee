#pragma once

namespace wee {
    enum struct clear_options : uint8_t {
        kClearColor     = 0b0001,
        kClearDepth     = 0b0010,
        kClearStencil   = 0b0100
    };
    struct graphics_device {
        SDL_Renderer* _renderer;

        graphics_device(SDL_Renderer* renderer) 
        : _renderer(renderer) {

        }

        virtual void clear(const SDL_Color* color, const clear_options&, float depth, int stencil) {
            constexpr static float val = 1.0f / 255;
            glClearDepthf(depth);
            glClearStencil(stencil);
            glClearColor(color->r * val, color->g * val, color->b * val, color->a * val);

            GLenum glClearOptions= GL_NONE;
            glClearOptions |= clear_options & kClearColor ? GL_COLOR_BUFFER_BIT : GL_NONE;
            glClearOptions |= clear_options & kClearDepth ? GL_DEPTH_BUFFER__BIT : GL_NONE;
            glClearOptions |= clear_options & kClearStencil ? GL_STENCIL_BUFFER_BIT : GL_NONE;
            glClear(glClearOptions);
        }
        virtual void set_rendertarget(SDL_Texture* t) {
            SDL_SetRenderTarget(_renderer, t);
        }

        virtual void draw_primitives(const primitive_type&, int start, size_t count, size_t primitiveCount) {
            for(auto i: range(primitiveCount)) {
                glDrawArrays(glGetPrimitiveType(type), start + count * i, count);
            }
        }
        virtual void draw_indexed_primitives(const primitive_type& type, 
            int baseVertex, 
            int minIndex, 
            size_t count, 
            int indexOffset, 
            size_t primitiveCount) 
        {
            for(auto i: range(primitiveCount)) {
                /*glDrawRangeElementsBaseVertex(glGetPrimitiveType(type),
                    minIndex,
                    minIndex + count,
                    count,
                    glGetIndexType(0),
                    BUFFER_OFFSET(indexOffset),
                    baseVertex);
                    */
                glDrawElements(glGetPrimitiveType(type),
                    count,
                    glGetIndexType(0),
                    BUFFER_OFFSET(minIndex)
                );
            }
        }
    };
}
