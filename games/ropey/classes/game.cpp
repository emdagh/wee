#include <classes/game.hpp>
#include <classes/screens.hpp>
#include <classes/input.hpp>
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
        {
            std::ifstream is = wee::open_ifstream("assets/textures.json");
            json j = json::parse(is);
            for(const auto& it : j) {
                std::ifstream ims = wee::open_ifstream(it["path"]);
                assets<SDL_Texture>::instance().load(it["name"], ims);
                ims.close();
            }
        }
        {
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
    //gamescreen::handle_all_input();
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

void game::set_callbacks(application* app) {
    app->on_mousedown += [&] (char) {
        input::instance().mouse_down = true;
        return 0;
    };
    app->on_mouseup += [] (char) {
        input::instance().mouse_down = false;
        return 0;
    };
    app->on_mousemove += [] (int x, int y) {
        input::instance().mouse_x = x;
        input::instance().mouse_y = y;
        
        return 0;
    };
}
