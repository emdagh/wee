#include <classes/screens.hpp>
#include <engine/assets.hpp>
#include <nlohmann/json.hpp>
#include <SDL.h>

using namespace wee;
using nlohmann::json;

splash_screen::splash_screen() {
}

splash_screen::~splash_screen () {
}

void splash_screen::load_content() {
    std::ifstream is = wee::open_ifstream("assets/splash.json");
    //json j(is);
    
    /*auto* _ = assets<SDL_Texture>::instance().load("@splash", 
        ::as_lvalue(wee::open_ifstream("assets/img/fa-image.png"))
    );*/

}
