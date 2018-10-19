#pragma once

#include <core/circular_array.hpp>
#include <core/delegate.hpp>
#include <core/logstream.hpp>
#include <SDL.h>
#include <exception>
#include <iterator>
#include <memory>

namespace wee {

    struct particles_base {
        virtual void emit(float, float, int, const SDL_Color&) = 0;
        virtual void update(int) = 0;
    };

    template <typename T>
    class particles : public particles_base {
    public:
        struct particle {
            float x,y;
            int t;
            int ttl;
            SDL_Color color;
            T state;
        };

        typedef delegate<void(particle&, int)> update_callback;
        typedef circular_array<particle> container_type;

        delegate<void(const particle&)> on_draw;
        update_callback _on_update;

        container_type _particles;
    public:
        particles(size_t capacity, const update_callback& on_update) 
        : _on_update(on_update)
        {
            _particles.reserve(capacity); 
        }
        void update(int dt) {
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


        void draw(const std::function<void(const container_type&)>& fn) {
            fn(_particles);
            /*static const int w = 32;
            static const int h = w;
            for(size_t i=0; i < _particles.size(); ++i) {
                particle& ref = _particles[i];//index_of(i)];

                SDL_Rect rc = { 
                    ref.x - w / 2, 
                    ref.y - h / 2, 
                    w,
                    h
                };
                SDL_SetRenderDrawColor(renderer, ref.color.r, ref.color.g, ref.color.b, ref.color.a);
                SDL_RenderFillRect(renderer, &rc);
            }*/
        }

        void emit(float x, float y, int ttl, const SDL_Color& color) {
            emit({x, y, 0, ttl, color, T::_});
        }

        void emit(const particle& p) {
            _particles.push_back(p);
        }

    };

    /*template <typename T>
    std::ostream& operator << (std::ostream& os, const typename particles<T>::particle& p) {
        return os;
    }*/
}
