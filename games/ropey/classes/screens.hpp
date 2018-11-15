#pragma once

#include <wee/wee.hpp>
#include <engine/gui/gamescreen.hpp>
#include <vector>
#include <core/factory.hpp>

struct SDL_Texture;
using nlohmann::json;

class splash_screen : public wee::gamescreen {
    int _visibleTime= 0;
    int _visibleTimeout = 1000;
    size_t _current= 0;
    std::vector<SDL_Texture*> _screens;
public:
    splash_screen();
    virtual ~splash_screen ();
    virtual void load_content(); 
    virtual void from_json(const json&);
    virtual void update(int, bool, bool);
    virtual void draw(SDL_Renderer*);
};

class main_menu_screen : public wee::gamescreen {
public:
    main_menu_screen();
    virtual ~main_menu_screen();
    virtual void load_content();
    virtual void from_json(const json&);

};

typedef wee::factory<wee::gamescreen> gamescreen_factory;

