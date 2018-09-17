#include <wee/wee.hpp>
#include <util/logstream.hpp>
#include <base/SDL_Application.h>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <fstream>
#include <engine/assets.hpp>
#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <engine/particles.hpp>

using namespace wee;

template <typename T>
T random(T lower, T upper) {
    static std::mt19937 eng; // This is the Mersenne Twister
    std::uniform_real_distribution<T> dist(lower, upper);
    return dist(eng);
}





SDL_Rect SDL_ScaleRect(const SDL_Rect& r, float scale) {
    return (SDL_Rect) {
        (int)((r.x - (r.w >> 1) * scale) + 0.5f),
        (int)((r.y - (r.h >> 1) * scale) + 0.5f),
        (int)((r.w * scale) + 0.5f),
        (int)((r.h * scale) + 0.5f)
    };
}

enum class Component : uintptr_t {
    None = 0,
    Drawable = 1 << 0,
    Transform = 1 << 1,
    CameraTrackable = 1 << 2,
    Timeout = 1 << 3,
    Tween = 1 << 4
};

#define MAX_ENTITIES    1024

struct DrawableComponent { 
    SDL_Texture* texture;
    SDL_Rect src;
    SDL_Rect dst;
};


struct TransformComponent {
    float x, y;
    float rotation;
};

struct World {
    uint32_t mask_[MAX_ENTITIES];
    DrawableComponent drawable_[MAX_ENTITIES];
    TransformComponent transform_[MAX_ENTITIES];

    
};



struct pstate {
    float vx, vy;

    static pstate _; 
};

pstate pstate::_ = { 0.f, 0.f }; 

struct particle_helper {
    static void explode(emitter<pstate>* em, float x, float y, int count) {
       
        float spawnRadius = 32.0f;
        for(int i=0; i < count; i++) {
            em->emit((emitter<pstate>::particle)
            {
                x + random(-spawnRadius, spawnRadius),
                y + random(-spawnRadius, spawnRadius), 
                0, 
                1000 + (int)random(0.0f, 1000.0f),
                SDL_CreateColorRGB(255, 255, 255),
                { 
                    random(-1.f, 1.f) * random(0.f, 2.f), 
                    random(-1.f, 1.f) * random(0.f, 2.f) 
                } 
            });
        }
    }
};

class game : public applet {
    SDL_Texture* texture;
    emitter<pstate>* p;
public:

    game() {
        p = new emitter<pstate>(1000, 

            [&] (emitter<pstate>::particle& p, int dt) {
                p.x += p.state.vx * dt;
                p.y += p.state.vy * dt;

                p.state.vx *= 0.8;
                p.state.vy *= 0.8;

                p.color.a = (uint8_t)(1.0f - ((float)p.t / p.ttl) * 255);
        });
    }

    virtual ~game() {}

    virtual int load_content() {
        DEBUG_METHOD();
        texture = asset_helper::from_file<SDL_Texture>("@face", "awesome_face.png");
        return 0;
    }

    virtual int update(int dt) {
        static int t = 0;
        t += dt;
        if(t >= 1000) {
            particle_helper::explode(p, 320.f, 240.f, 128); 
            t = 0;
        }

        p->update(dt);
        return 0;
    }

    virtual int draw(SDL_Renderer* renderer) {
        static SDL_Texture* fb = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, 640, 480);

        static int once = 0;

        /**
         * render the scene 
         */
        {
            SDL_SetRenderTarget(renderer, fb);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);//IndianRed);
            SDL_RenderClear(renderer);
            //SDL_RenderCopy(renderer, texture, NULL, NULL);
            p->draw(renderer);
        }


        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, fb, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
        return 0;
    }

};

extern applet* wee_CreateGame() {

    return new game;
}


int main(int , char* []) {
    DEBUG_METHOD();
    try {
        application app(wee_CreateGame());
        app.resize(640, 480);

        return app.start();
    } catch(const file_not_found& e) {
        LOGE(e.what());
    } catch(...) {
        std::cout << "exception caught" << std::endl;
    }
}

static class SDL_Initializer {
    public:
        SDL_Initializer() { 
            DEBUG_METHOD();
            SDL_Init(SDL_INIT_EVERYTHING);
        }

        virtual ~SDL_Initializer() {
            DEBUG_METHOD();
            SDL_Quit();
        }
} _;
