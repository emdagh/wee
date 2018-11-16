#pragma once

#include <Box2D/Box2D.h>
#include <SDL.h>
#include <core/vec2.hpp>
#include <engine/ecs.hpp>
#include <engine/b2DebugDrawImpl.hpp>
#include <engine/b2ContactListenerImpl.hpp>
#include <engine/camera.hpp>
#include <engine/gui/gamescreen.hpp>
#include <classes/common.hpp>

namespace wee {
    struct application;
}

class gameplay_screen : public wee::gamescreen {
    b2World* _world;
    bool _mousedown = false;
    int _time_down = 0;
    SDL_Rect _camera;
    entity_type b0, p, b1;
    kult::type _rope;
    wee::b2DebugDrawImpl _debugdraw;
    wee::camera _cam;
    wee::b2ContactListenerImpl _contacts;
public:
    gameplay_screen();
    virtual ~gameplay_screen();
    virtual void load_content();
    virtual void handle_input();
    virtual void update(int, bool, bool);
    virtual void draw(SDL_Renderer*);
public:
    int on_click() ;
};
