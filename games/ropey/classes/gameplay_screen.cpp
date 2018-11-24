#include <classes/gameplay_screen.hpp>
#include <classes/systems.hpp>
#include <classes/entities.hpp>
#include <classes/factories.hpp>
#include <classes/components.hpp>
#include <base/application.hpp>
#include <core/logstream.hpp>
#include <engine/b2RayCastImpl.hpp>
#include <engine/assets.hpp>
#include <engine/ecs.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <classes/tmx.hpp>
#include <classes/input.hpp>
#include <classes/level.hpp>


using namespace wee;
//typedef factory<entity_type, std::string,        b2World*, const tmx::Object&> object_factory;
//typedef factory<b2Shape*,   tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;

gameplay_screen::gameplay_screen() {
    _debugdraw.SetFlags(
            b2Draw::e_shapeBit          | //= 0x0001, 
            b2Draw::e_jointBit          | //= 0x0002, 
            //b2Draw::e_aabbBit           | //= 0x0004, 
            //b2Draw::e_pairBit           | //= 0x0008, 
            b2Draw::e_centerOfMassBit   | //= 0x0010, 
            //e_particleBit// = 0x0020 
            0
            ) ;
    _world = new b2World({0.0f, 9.8f});
    _world->SetDebugDraw(&_debugdraw);
    _world->SetContactListener(&_contacts);
    _camera = { 0, 0, 0, 0 };
    this->_time_on = 4500;
}
gameplay_screen::~gameplay_screen() {
}

void rotate(const vec2f& in, const vec2f& at, float t, vec2f* out) {
    float c = std::cos(t);
    float s = std::sin(t);

    out->x = at.x + (in.x * c - in.x * s);
    out->y = at.y + (in.y * s + in.y * c);
}

void nested_to_transform() {
    for(auto& self : kult::join<nested, transform>()) {
        const auto& n = kult::get<nested>(self);
        if(kult::has<transform>(n.parent)) {
            const auto& pt = kult::get<transform>(n.parent);

            vec2f pos = vec2f::rotate_at(n.offset, pt.position, pt.rotation);

            kult::get<transform>(self).position = pos;
            kult::get<transform>(self).rotation = kult::get<transform>(n.parent).rotation + n.rotation;
        } else {
            kult::get<transform>(self).position = n.offset;
            kult::get<transform>(self).rotation = n.rotation;
        }
    }
}

void disable_and_hide(const entity_type& self) {
    for(const auto& child : kult::join<nested>()) {
        if(self == kult::get<nested>(child).parent) {
            disable_and_hide(child);
        }
    }
    /**
     * disable
     */
    if(kult::has<physics>(self)) {
        kult::get<physics>(self).body->SetActive(false);
    }
    /**
     * and hide
     */
    if(kult::has<visual>(self)) {
        kult::get<visual>(self).visible = false;
    }
    if(kult::has<pickup>(self)) {
        kult::get<pickup>(self).active = false;
    }
}
void enable_and_show(const entity_type& self) {
    for(const auto& child : kult::join<nested>()) {
        if(self == kult::get<nested>(child).parent) {
            enable_and_show(child);
        }
    }
    /**
     * enable..
     */
    if(kult::has<physics>(self)) {
        kult::get<physics>(self).body->SetActive(true);
    }
    /**
     * and show
     */
    if(kult::has<visual>(self)) {
        kult::get<visual>(self).visible = true;
    }

    if(kult::has<pickup>(self)) {
        kult::get<pickup>(self).active = true;
    }
}

void gameplay_screen::load_content() {
    std::ifstream ifs = wee::open_ifstream("assets/levels.json");
    json j = json::parse(ifs);
    for(const auto& i : j) {
        std::string abs_path = get_resource_path(dirname(i)) + basename(i);
        tmx::Map tiled_map;
        tiled_map.load(abs_path);

        level builder;
        auto id = builder.source(&tiled_map)
            .world(_world)
            .build();
        _beats.push_back(id);
        disable_and_hide(id);
    }
    _current_beat_idx = 0;
    enable_and_show(_beats[_current_beat_idx]);
    
    vec2f spawnPoint = kult::get<beat>(_beats[_current_beat_idx]).spawn;
    p  = create_player(_world, spawnPoint);

    _cam.set_zoom(1.f);

    _restart();
}

void gameplay_screen::handle_input() {
    static const int clickTimeout = 500;
    static int mouseWasDown = 0;
    static int mouseDownTime = 0;
    if(input::instance().mouse_down) {
        if(mouseWasDown == 0) {
            mouseDownTime = SDL_GetTicks();
        }
        mouseWasDown = 1;
    } else {
        if(mouseWasDown) {
            mouseWasDown = 0;
            auto delta = SDL_GetTicks() - mouseDownTime;
            if((delta) < clickTimeout) {
                on_click();
            }
        }
    }
}

void gameplay_screen::_restart() {
    auto _current_beat = _beats[_current_beat_idx];
    const auto& beatTransform = kult::get<transform>(_current_beat);
    beat_t& b = kult::get<beat>(_current_beat);
    b.respawn++;
    kult::get<transform>(p).position = b.spawn + beatTransform.position;
    kult::get<physics>(p).body->SetLinearVelocity(b2Vec2(0, 0));
    kult::get<physics>(p).body->SetAngularVelocity(0);
    
    copy_transform_to_physics();

    b2Vec2 pa = kult::get<physics>(p).body->GetPosition();
    b2Vec2 temp = { 0.0f, -1000.0f };
    b2Vec2 pb = pa + SCREEN_TO_WORLD(temp);
    
    b2RayCastClosest rc;
    rc.RayCast(_world, pa, pb);
}


void gameplay_screen::update(int dt, bool a, bool b) {
    nested_to_transform();
    copy_transform_to_physics();
    _world->Step(1.0f / (float)60, 4, 3);
    copy_physics_to_transform();
    
    auto _current_beat = _beats[_current_beat_idx];
     
    b2Vec2 playerPos = WORLD_TO_SCREEN(kult::get<physics>(p).body->GetPosition());

    for(auto& e : kult::join<raycast, nested>()) {
        auto& r = kult::get<raycast>(e);
        //auto& n = kult::get<nested>(e);

        if(r.hit) {
            r.hit = false;
            _rope = create_rope(_world, p, e, b2Vec2{r.point.x, r.point.y});
           
            //if(_current_beat != n.parent) {
            //    _spawnedNextBeat = false;
            //    DEBUG_LOG("new beat entered");
            //}
            break;
        }
    }
    /**
     * rules for spawning a new beat:
     *  + player should be across half of the current beat. (px >= beat.width / 2)
     *  + there isn't already a beat spawned after the current beat.
     */
    auto& current_tx = kult::get<transform>(_current_beat);
    auto& current_bt = kult::get<beat>(_current_beat);

    if(playerPos.x > (current_tx.position.x + current_bt.width * 0.5f)) {
        //if(playerPos.x > (current_tx.position.x + current_bt.width)) {
        //    _spawnedNextBeat = false;
        //}
        //if(!_spawnedNextBeat) {
            _spawnedNextBeat = true;
            auto next_beat_idx = (_current_beat_idx + 1) % _beats.size();//_get_next_beat(_current_beat);
            enable_and_show(_beats[next_beat_idx]);
            auto& next_tx = kult::get<transform>(_beats[next_beat_idx]);
            next_tx.position.x = current_tx.position.x + current_bt.width;
            _current_beat_idx = next_beat_idx;
            //nested_to_transform();
            //copy_transform_to_physics();
        //}
    }


    _cam.set_position(playerPos.x, playerPos.y);
    _cam.update(dt);
    _debugdraw.SetCameraTransform(_cam.get_transform());
    synchronize_entities();
    gamescreen::update(dt, a, b);
}



void gameplay_screen::draw(SDL_Renderer* renderer) {
    _debugdraw.SetRenderer(renderer);

    SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);

    _cam.set_viewport(_camera.w, _camera.h);
    {
        for(const auto& e : kult::join<transform, visual>()) {
           
            //const auto& n = kult::get<nested>(e);
            const visual_t& v = kult::get<visual>(e);
            const transform_t& t = kult::get<transform>(e);

            if(!v.visible)
                continue;

            vec3 position = { 
                t.position.x, 
                t.position.y,
                0.0f
            };

            vec3 positionCS = vec3::transform(position, _cam.get_transform());

            vec3 size = vec3{
                (float)v.src.w, 
                (float)v.src.h, 
                0.0f
            };
            
            size = size * _cam.get_zoom(); 

            
            SDL_Rect dst = {
                (int)((positionCS.x + 0.5f) - (size.x * 0.5f)), 
                (int)((positionCS.y + 0.5f) - (size.y * 0.5f)),
                (int)size.x, 
                (int)size.y
            };
            SDL_RenderCopyEx(renderer, 
                    v.texture,
                    &v.src,
                    &dst,
                    t.rotation,
                    NULL,
                    v.flip
                    );
        }

        /*std::sort(entities.begin(), entities.end(), [&] (const kult::type& a, const kult::type& b) {
          int la = kult::get<visual>(a).layer;
          int lb = kult::get<visual>(b).layer;
          return la < lb;
          });*/
    }
    //b2DebugDrawEXT(_world, renderer, _camera);
    //
    _world->DrawDebugData();

    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);

    vec2 pa = kult::get<transform>(p).position;
    vec3 playerPos = vec3::transform ({pa.x, pa.y, 0.0f}, _cam.get_transform());
    pa.x = playerPos.x;
    pa.y = playerPos.y;

    vec2 pb = { 
        (float)input::instance().mouse_x, 
        (float)input::instance().mouse_y 
    };


    SDL_RenderDrawLine(renderer, 
            (int)pa.x, 
            (int)pa.y, 
            (int)pb.x, 
            (int)pb.y
            );

    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
    gamescreen::draw(renderer);

}

int gameplay_screen::on_click() {

    joint_t& a = kult::get<joint>(_rope);
    if(a.joint) {
        DEBUG_LOG("destroy old joint");
        _world->DestroyJoint(a.joint);
        a.joint = NULL;
    }

    for(auto& e : kult::join<raycast>()) {
        kult::get<raycast>(e).hit = false;
    }

    vec2 playerPosition = kult::get<transform>(p).position;

    vec3 mousePosition = {
        (float)input::instance().mouse_x,
        (float)input::instance().mouse_y,// - _camera.h / 2, 
        0.0f
    };

    mousePosition = vec3::transform(mousePosition, mat4::inverted(_cam.get_transform()));

    b2Vec2 pa = SCREEN_TO_WORLD(b2Vec2(playerPosition.x, playerPosition.y));
    b2Vec2 pb = SCREEN_TO_WORLD(b2Vec2(mousePosition.x, mousePosition.y));


    b2RayCastClosest rc;
    rc.RayCast(_world, pa, pb);
    _cam.shake(1000, false);
    return 0;
}

