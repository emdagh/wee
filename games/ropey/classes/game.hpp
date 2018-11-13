#include <base/applet.hpp>
#include <Box2D/Box2D.h>
#include <SDL.h>
#include <core/vec2.hpp>
#include <engine/ecs.hpp>
#include <engine/b2DebugDrawImpl.hpp>
#include <engine/b2ContactListenerImpl.hpp>
#include <engine/camera.hpp>
#include <classes/common.hpp>

namespace wee {
    struct application;
}

class game : public wee::applet {
    b2World* _world;
    bool _mousedown = false;
    int _time_down = 0;
    SDL_Rect _camera;
    entity_type b0, p, b1;
    wee::vec2f _mouse_pos;
    kult::type _rope;
    wee::b2DebugDrawImpl _debugdraw;
    wee::camera _cam;
    wee::b2ContactListenerImpl _contacts;
public:
    game();
    int load_content(); 
    int update(int dt) ;
    int draw(SDL_Renderer* renderer) ;
    int on_click() ;
    void callbacks(wee::application* app) ;

};
