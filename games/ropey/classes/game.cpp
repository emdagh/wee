#include <classes/game.hpp>
#include <classes/screens.hpp>
#include <base/application.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <engine/assets.hpp>
#include <nlohmann/json.hpp>

using namespace wee;
using nlohmann::json;

game::game() {
}

game::~game() {
}

int game::load_content() {
    try {
        std::ifstream is = wee::open_ifstream("assets/screens.json");
        json j = json::parse(is);
        for(const auto& it : j) {
            gamescreen* s = gamescreen_factory::instance().create(it["class"]);
            if(s != NULL) {
                s->from_json(it);
                _screens.push_back(s);
                _screen_index[it["name"]] = _screens.size() - 1;
            }
        }
    } catch(...) {
        DEBUG_LOG("caught unhandled exception...");
    }

    if(_screens.size() > 0) {
        gamescreen::add(_screens[0]);
    }

    //gamescreen::add(new splash_screen);
    return 0;
}

int game::update(int dt) {
    gamescreen::update_all(dt);
    
    return 0;

}

int game::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
    SDL_RenderClear(renderer);
    gamescreen::draw_all(renderer);
    SDL_RenderPresent(renderer);
    return 0;
}

void game::set_callbacks(application* ) {
    /*
    app->on_mousedown += [&] (char) {
        //this->_mousedown = true;
        //this->_time_down = SDL_GetTicks();
        return 0;
    };

    app->on_mouseup += [this] (char) {
        this->_mousedown = false;
        int dt = (SDL_GetTicks() - this->_time_down);
        static const int TIMEOUT = 500;
        DEBUG_VALUE_OF(dt);
        if(dt < TIMEOUT) {
            return this->on_click();
        }
        return 0;
    };

    app->on_mousemove += [this] (int , int ) {
        
        this->_mouse_pos.x = (float)x;
        this->_mouse_pos.y = (float)y;
        
        return 0;
    };
    */
}
