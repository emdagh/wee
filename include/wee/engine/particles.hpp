#pragma once

#include <core/circular_array.hpp>
#include <core/delegate.hpp>
#include <util/logstream.hpp>
#include <SDL.h>
#include <exception>
#include <iterator>
#include <memory>

namespace wee {

    struct emitter_base {
        virtual void emit(float, float, int, const SDL_Color&) = 0;
        virtual void update(int) = 0;
        virtual void draw(SDL_Renderer*) = 0; //, SDL_Texture*) = 0;
    };

    template <typename T>
    class emitter : public emitter_base {
    public:
        struct particle {
            float x,y;
            int t;
            int ttl;
            SDL_Color color;
            T state;
        };

        typedef delegate<void(particle&, int)> update_callback;

        delegate<void(const particle&)> on_draw;
        update_callback _on_update;

        circular_buffer<particle> _particles;
    public:
        emitter(size_t capacity, update_callback on_update) 
        : _on_update(on_update)
        {
            _particles.reserve(17); 
        }
        void update(int dt) {
            int n = 0;
            for(size_t i=0; i < _particles.size(); i++) {
                particle& ref = _particles[i];
                ref.t += dt;
                _on_update(ref, dt);
            }
            _particles.erase(
                std::remove_if(_particles.begin(), _particles.end(), [&] (const particle& ref) {
                    return ref.t >= ref.ttl;
                }), 
                _particles.end()
            );
        }


        void draw(SDL_Renderer* renderer) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            static const int w = 32;
            static const int h = w;
            for(size_t i=0; i < _particles.size(); ++i) {
                particle& ref = _particles[i];//index_of(i)];

                SDL_Rect rc = { 
                    ref.x - w / 2, 
                    ref.y - h / 2, 
                    w,
                    h
                };
                //SDL_SetTextureColorMod(tex, p->color.r, p->color.g, p->color.b);
                //SDL_SetTextureAlphaMod(tex, p->color.a);
                SDL_SetRenderDrawColor(renderer, ref.color.r, ref.color.g, ref.color.b, ref.color.a);
                SDL_RenderFillRect(renderer, &rc);
            }
        }

        void emit(float x, float y, int ttl, const SDL_Color& color) {
            emit({x, y, 0, ttl, color, T::_});
        }

        void emit(const particle& p) {
            _particles.push_back(p);
        }

    };

    /*template <typename T>
    std::ostream& operator << (std::ostream& os, const typename emitter<T>::particle& p) {
        return os;
    }*/
}
