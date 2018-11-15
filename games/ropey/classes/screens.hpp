#pragma once

#include <wee/wee.hpp>
#include <engine/gui/gamescreen.hpp>
#include <vector>
#include <core/factory.hpp>

struct SDL_Texture;

class splash_screen : public wee::gamescreen {
    std::vector<SDL_Texture*> _images;
public:
    splash_screen();
    virtual ~splash_screen ();
    void load_content(); 
};

class main_menu_screen : public wee::gamescreen {
public:
    main_menu_screen();
    virtual ~main_menu_screen();
    void load_content();
};

typedef wee::factory<wee::gamescreen> gamescreen_factory;

