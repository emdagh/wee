#include <classes/screens.hpp>
#include <engine/assets.hpp>
#include <nlohmann/json.hpp>
#include <SDL.h>

using namespace wee;

splash_screen::splash_screen() {
    DEBUG_METHOD();
}

splash_screen::~splash_screen () {
}

void splash_screen::update(int dt, bool otherScreenHasFocus, bool) {

    static bool coveredByOtherScreen = false;
    
    if(state() == E_STATE_ACTIVE) {
        _visibleTime += dt;
        if(_visibleTime >= _visibleTimeout) {
            coveredByOtherScreen = true;
        }
    }
    else if(state() == E_STATE_HIDDEN) {
        coveredByOtherScreen = false;
        _current++;
        _visibleTime = 0;
    }
    
    if(_current >= _screens.size()) {
        coveredByOtherScreen = true;
        quit();
    }

    gamescreen::update(dt, otherScreenHasFocus, coveredByOtherScreen);
}

void splash_screen::draw(SDL_Renderer* renderer) {
    int rw, rh;
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
   
    auto* tex = _screens[_current];
    int tw, th;
    SDL_QueryTexture(tex, NULL, NULL,  &tw, &th);
    SDL_Rect rc = {
        rw / 2 - tw / 2,
        rh / 2 - th / 2,
        std::min(rw, tw),
        std::min(rh, th)
    };

    uint8_t alpha = ((1.0f - transition()) * 255.f);
    SDL_SetTextureAlphaMod(tex, alpha);
    SDL_RenderCopy(renderer, tex, NULL, &rc);
    gamescreen::draw(renderer);
}

void splash_screen::from_json(const json& j) {

    //std::map<std::string, json> m = j;
    for(const auto& kv : j["screens"]) {
        DEBUG_VALUE_OF(kv);
        _screens.push_back(
            assets<SDL_Texture>::instance().load(kv["name"], as_lvalue(wee::open_ifstream(kv["uri"])))
        );
    }
    
    //SDL_Texture* s = assets<SDL_Texture>::instance().load("@splash_0",
    //    open_ifstream("assets/img/fa-image.png")
    //);
    gamescreen::from_json(j);
}

void splash_screen::load_content() {
    //std::ifstream is = wee::open_ifstream("assets/splash.json");
    
    /*json tmp = { 
        { "transition_on", 1.5 },
        { "transition_off", 1.5 },

        "screens" , {
            { "@splash_0", "assets/img/fa-image.png" },
            { "@splash_1", "assets/img/fa-image.png" }
        }
    };*/
    //from_json(tmp);
    //json j(is);
    
    /*auto* _ = assets<SDL_Texture>::instance().load("@splash", 
        ::as_lvalue(wee::open_ifstream("assets/img/fa-image.png"))
    );*/

}
