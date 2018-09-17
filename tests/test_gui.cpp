#include <engine/gui/layout.hpp>
#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <engine/gui/layout.hpp>
#include <util/logstream.hpp>
#include <SDL.h>

#define GEN_ID  __LINE__

namespace imgui {

    /*using type = int;

    template <typename T = type>
    T& id() {
        static T _id = T();
        return ++_id;
    }*/

    struct state {
        int hot;
        int active;
        int focus;
        int last;

        int mousex;
        int mousey;
        int mousedown;

        typedef std::function<void(int, const SDL_Rect&)> callback;

        callback _draw;
        callback _draw_hot;
        callback _draw_active;
        callback _draw_hot_and_active;

    } uistate = {0,0,0,0,0,0,0, NULL, NULL, NULL, NULL};

    int prepare() {
        uistate.hot = 0;
        uistate.active = 0;
        uistate.focus = 0;
        uistate.last = 0;
    }

    int regionhit(const SDL_Rect& r) {
        return 
            (uistate.mousex >= r.x) && 
            (uistate.mousex < r.x + r.w) &&
            (uistate.mousey >= r.y) &&
            (uistate.mousey < r.h + r.h);
    }

    int _before(int id, const SDL_Rect& r) {
        if(regionhit(r)) {
            uistate.hot = id;
            if(uistate.active == 0 && uistate.mousedown) {
                uistate.active = id;
            }
        }

        if(uistate.focus == 0) {
            uistate.focus = id;
        }

        if(uistate.focus == id) {
            //focus_callback(r);
        }
    }

    int _after(int id, const SDL_Rect& r) {
        if(uistate.hot == id) {
            if(uistate.active == id) {
                uistate._draw_hot_and_active(id, r);
            } else {
                uistate._draw_hot(id, r);
            }
        } else {
            uistate._draw_active(id, r);
        }
    }

    int button(int id, const SDL_Rect& r) {
        _before(id, r);
        uistate._draw(id, r);
        _after(id, r);
        uistate.last = id;

        if(uistate.mousedown == 0 &&
                uistate.hot == id &&
                uistate.active == id) {
            return 1;
        }
        return 0;
    }
    int slider(int id, const SDL_Rect& r, float value_max, float* value, bool horizontal=false) {

        _before(id, r);

        float val_n = std::min(std::max(*value, 0.0f), value_max) / value_max;
        uistate._draw(id, r);
        
        int w = std::min(r.w, r.h) >> 1;
        SDL_Rect grip = { 0, 0, w, w };

        if(horizontal) {
            grip = {
                r.x + static_cast<int>((r.w - r.h) * val_n),
                r.y,
                r.h, 
                r.h
            };
        } else {
            grip = {
                r.x,
                (r.y + r.h) - r.w - static_cast<int>((r.h - r.w) * val_n),
                r.w, 
                r.w
            };
        }

        _after(id, r);

        if(uistate.active || uistate.hot) {
            uistate._draw_active(id, grip);
        } else {
            uistate._draw(id, grip);
        }

        if(uistate.active == id) {
            if(horizontal) {
                float mouse_rel = uistate.mousex - (r.x + grip.w / 2);
                mouse_rel = std::max(0.f, std::min(mouse_rel, (float)r.w - grip.w));
                *value = mouse_rel / (r.w - grip.w) * value_max;
                return 1;
            } else {
                return 1;
            }
        }
        return 0;

    }
    int textfield();
}

using namespace wee;

struct game : applet {
    std::vector<SDL_Rect*> _rects;
    border_layout _layout;
    SDL_Renderer* _renderer;
    
    void set_callbacks(application* app) {
        app->on_mousemove += [&] (int x, int y) {
            DEBUG_LOG("mouse={},{}", x, y);
            return 0;
        };

    }

    virtual int load_content() {
        for(int i=0; i < 5; i++) {
            SDL_Rect* r = new SDL_Rect;
            r->w = 128;
            r->h = 64;
            _rects.push_back(r);
            _layout.add(r, (border_layout::location)i);
        }
        _layout.apply({0, 0, 640, 480});
    } 
    virtual int update(int) {

    }
    virtual int draw(SDL_Renderer* renderer) {
        if(!_renderer) {
            _renderer = renderer;
            imgui::uistate._draw = [&] (int, const SDL_Rect& r) {
                SDL_RenderDrawRect(_renderer, &r);
            };
            imgui::uistate._draw_hot = [&] (int, const SDL_Rect& r) {
                SDL_RenderDrawRect(_renderer, &r);
            };
            imgui::uistate._draw_active = [&] (int, const SDL_Rect& r) {
                SDL_RenderDrawRect(_renderer, &r);
            };
            imgui::uistate._draw_hot_and_active = [&] (int, const SDL_Rect& r) {
                SDL_RenderDrawRect(_renderer, &r);
            };
        }

        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
        for(auto* r : _rects) {
            SDL_RenderDrawRect(renderer, r);
        }

        imgui::prepare();
        imgui::button(GEN_ID, { 0, 0, 64, 32 });

        SDL_RenderPresent(renderer);
    }
};

int main(int, char* []) {

    applet* let = new game;
    application app(let);
    ((game*)let)->set_callbacks(&app);
    return app.start();
}
