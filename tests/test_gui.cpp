#include <engine/gui/layout.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <engine/gui/layout.hpp>
#include <core/logstream.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <functional>

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

        return 0;
    }

    int regionhit(const SDL_Rect& r) {
        return 
            (uistate.mousex >= r.x) && 
            (uistate.mousex < r.x + r.w) &&
            (uistate.mousey >= r.y) &&
            (uistate.mousey < r.y + r.h);
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
        return id;
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
        return id;
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
        
        //int w = std::min(r.w, r.h) >> 1;
        SDL_Rect grip;

        if(horizontal) {
            grip = { r.x + static_cast<int>((r.w - r.h) * val_n), r.y, r.h, r.h };
        } else {
            grip = { r.x, (r.y + r.h) - r.w - static_cast<int>((r.h - r.w) * val_n), r.w, r.w };
        }

        _after(id, r);

        if(uistate.active == id || uistate.hot == id) {
            uistate._draw_active(id, grip);
        } else {
            uistate._draw_hot(id, grip);
        }

        if(uistate.active == id) {
            if(horizontal) {
                float mouse_rel = uistate.mousex - (r.x + grip.w / 2);
                mouse_rel = std::max(0.f, std::min(mouse_rel, (float)r.w - grip.w));
                *value = mouse_rel / (r.w - grip.w) * value_max;
                return 1;
            } else {
                float mouse_rel = uistate.mousey - (r.y + grip.h / 2);
                mouse_rel = std::max(0.f, std::min(mouse_rel, (float)r.h - grip.h));
                *value = value_max - mouse_rel / (r.h - grip.h) * value_max;
                return 1;
            }
        }
        return 0;

    }
    int textfield(int id, const SDL_Rect& rc);

    int graph(int id, const SDL_Rect& rc);
}

using namespace wee;

SDL_Rect SDL_GrowRect(const SDL_Rect& r, int s) {
    SDL_Rect res;
    res.x = r.x - (s >> 1);
    res.y = r.y - (s >> 1);
    res.w = r.w + s;// + (s >> 1);
    res.h = r.h + s;// + (s >> 1);
    return res;
}


struct game : applet {
    std::vector<SDL_Rect*> _rects;
    border_layout _borderlayout;
    flow_layout _flowlayout;
    SDL_Renderer* _renderer;
    float _value;

    SDL_Rect* _create_rect(int x, int y, int w, int h) {
        SDL_Rect* rc = new SDL_Rect;
        rc->x = x;
        rc->y = y;
        rc->w = w;
        rc->h = h;
        _rects.push_back(rc);
        return rc;
    }

    void set_callbacks(application* app) {
        app->on_mousemove += [&] (int x, int y) {
            imgui::uistate.mousex = x;
            imgui::uistate.mousey = y;
            return 0;
        };
        app->on_mousedown += [&] (uint16_t) {
            imgui::uistate.mousedown = 1;
            return 0;
        };
        app->on_mouseup += [&] (uint16_t) {
            imgui::uistate.mousedown = 0;
            return 0;
        };
    }

    virtual int load_content() {
        for(int i=0; i < 5; i++) {
            SDL_Rect* r = new SDL_Rect;
            r->w = 128;
            r->h = 64;
            _rects.push_back(r);
            _borderlayout.add(r, (border_layout::location)i);
        }
        _borderlayout.apply({0, 0, 640, 480});


        _value = 0.0f;
        return 0;
    } 

    virtual int update(int) {
        return 0;
    }

    virtual int draw(SDL_Renderer* renderer) {
        if(!_renderer) {
            _renderer = renderer;
            imgui::uistate._draw = [&] (int, const SDL_Rect& r) {
                SDL_SetRenderDrawColorEXT(_renderer, SDL_ColorPresetEXT::DarkGrey);
                SDL_RenderFillRect(_renderer, &r);
            };
            imgui::uistate._draw_hot = [&] (int, const SDL_Rect& r) {
                auto rbig = SDL_GrowRect(r, -6);
                SDL_SetRenderDrawColorEXT(_renderer, SDL_ColorPresetEXT::IndianRed);
                SDL_RenderDrawRect(_renderer, &rbig);
            };
            imgui::uistate._draw_active = [&] (int, const SDL_Rect& r) {
                auto rbig = SDL_GrowRect(r, 2);
                SDL_SetRenderDrawColorEXT(_renderer, SDL_ColorPresetEXT::GreenYellow);
                SDL_RenderDrawRect(_renderer, &rbig);
            };
            imgui::uistate._draw_hot_and_active = [&] (int, const SDL_Rect& r) {
                SDL_SetRenderDrawColorEXT(_renderer, SDL_ColorPresetEXT::Fuchsia);
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
        imgui::button(GEN_ID, { 128, 128, 64, 32 });
        imgui::slider(GEN_ID, { 256, 128, 16, 128}, 100.0f, &_value, false);

        SDL_RenderPresent(renderer);

        return 0;
    }
};

int main(int, char* []) {

    applet* let = new game;
    application app(let);
    ((game*)let)->set_callbacks(&app);
    return app.start();
}
