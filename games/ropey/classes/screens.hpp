#pragma once

#include <wee/wee.hpp>
#include <engine/gui/gamescreen.hpp>

class splash_screen : public gamescreen {
    std::vector<SDL_Texture*> _images;
public:

    void load_content() {
        std::ifstream is = asset_helper::open_stream("assets/splash.json");
        json j(is);

    }
};
