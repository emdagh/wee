#include "common/common.hpp"
#include "common/components.hpp"
#include "common/Box2DEXT.hpp"
#include <util/logstream.hpp>
#include <kult.hpp>
#include <Box2D/Box2D.h>

using namespace wee;

typedef kult::type entity_type;


class b2RayCastListener : public b2RayCastCallback {
    b2Fixture* _fixture;
    b2Vec2 _point, _normal;
    float _fraction= 1.f;
public:
    void RayCast(b2World* world, const b2Vec2& p1, const b2Vec2& p2) {
        DEBUG_METHOD();
        _fixture = NULL;
        _fraction = 1.0f;
        world->RayCast(this, p1, p2);
        DEBUG_LOG("end");
        if(_fixture) {
            kult::type self = reinterpret_cast<kult::type>(_fixture->GetUserData());
            raycast_t& r = kult::get<raycast>(self);
            r.is_hit = true;
            r.point = {_point.x, _point.y};
            r.normal = {_normal.x, _normal.y};
        }

    }
    float32 ReportFixture(b2Fixture* fixture, 
            const b2Vec2& point, 
            const b2Vec2& normal, 
            float32 fraction) {
        auto self = reinterpret_cast<kult::type>(fixture->GetUserData());
        raycast_t& r = kult::get<raycast>(self);
        _fixture    = fixture;
        _fraction   = fraction;
        _point      = point;
        _normal     = normal;
        
        return fraction;
    }
};


struct camera {
    SDL_Rect rect;
};

kult::type create_sensor(b2World* world, kult::type parent, const vec2f& offset, float radius, const collision_callback&) {
    kult::type self = kult::entity();
    b2Body* body = nullptr;
    b2Fixture* fixture = nullptr;
    {

        b2BodyDef def;
        def.type = b2_staticBody;
        body = world->CreateBody(&def);
    }
    {
        b2CircleShape shape;
        shape.m_radius = SCREEN_TO_WORLD(radius);
        b2FixtureDef def;
        def.shape = &shape;
        def.isSensor = true;
        fixture = body->CreateFixture(&def);

    }
    kult::add<collider>(self).fixture = fixture;
    kult::add<rigidbody>(self).body = body;
    kult::add<nested>(parent);
    {
        nested_t& n = kult::get<nested>(parent);
        n.parent = parent;
        n.offset = offset;
        
    }
    return self;
}

entity_type create_rope_between(b2World* world, entity_type a, entity_type b, const b2Vec2& bPosWS) {


    b2Body* body = kult::get<rigidbody>(b).body;
    if(!body) {
        throw std::logic_error("entity must have a rigidbody attached");
    }
    kult::type e = kult::entity();

    b2RopeJointDef def;
    def.userData = reinterpret_cast<void*>(e);
    def.bodyA = kult::get<rigidbody>(a).body; 
    def.bodyB = kult::get<rigidbody>(b).body; 
    def.collideConnected = true;
    // center of player
    def.localAnchorA = b2Vec2 {0.0f, 0.0f};
    // raycast the world here
    def.localAnchorB = body->GetLocalPoint(bPosWS);
    def.maxLength = (def.bodyA->GetPosition() - bPosWS).Length();//SCREEN_TO_WORLD(0.0f);

    auto* joint = world->CreateJoint(&def);
    kult::add<articulation>(e).joint = joint;//world->CreateJoint(&def);
    return e;
}


entity_type create_player(b2World* world, const SDL_Point& at) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
    b2Fixture* fixture = nullptr;
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody; 
        bd.position.Set(SCREEN_TO_WORLD(at.x), SCREEN_TO_WORLD(at.y));
        body = world->CreateBody(&bd);
    }

    {
        b2CircleShape shape;
        shape.m_p.Set(0.0f, 0.0f);//
        shape.m_radius = SCREEN_TO_WORLD(10.0f);
        b2FixtureDef fd;
        fd.filter.categoryBits = 0xffff;//!0;//(uint16_t)collision_filter::player;
        fd.filter.maskBits = 0xffff;//!0;//(uint16_t)collision_filter::any;
        fd.density = 1.0f;
        fd.restitution = 0.0f;
        fd.shape = &shape;
        fd.isSensor = false;
        fd.userData = reinterpret_cast<void*>(self);

        fixture = body->CreateFixture(&fd);
    }
    kult::add<rigidbody>(self).body = body;
    kult::add<collider>(self).fixture = fixture;
    return self;
}

entity_type create_block(b2World* world, const SDL_Rect& r) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
    b2Fixture* fixture=  nullptr;

    float half_w = (float)(r.w >> 1);
    float half_h = (float)(r.h >> 1);

    {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(
            SCREEN_TO_WORLD(r.x + half_w), 
            SCREEN_TO_WORLD(r.y + half_h)
        );
        body = world->CreateBody(&def);
    }

    {
        b2PolygonShape shape;
        shape.SetAsBox(
            SCREEN_TO_WORLD(half_w), 
            SCREEN_TO_WORLD(half_h)
        );
        b2FixtureDef def;
        def.shape = &shape;
        def.isSensor = false;
        def.filter.categoryBits = 0xffff;
        def.filter.maskBits = 0xffff;
        def.userData = reinterpret_cast<void*>(self);
        
        fixture = body->CreateFixture(&def);
    }

    kult::add<rigidbody>(self).body = body;
    kult::add<collider>(self).fixture = fixture;
    kult::add<raycast>(self).is_hit = false;

    return self;
}


class game : public applet {
    b2World* _world;
    bool _mousedown = false;
    int _time_down = 0;
    SDL_Rect _camera;
    entity_type b0, p, b1;
    vec2f _mouse_pos;
    b2RayCastListener _raycast;
    kult::type _rope;
public:
    game() {
        _world = new b2World({0.0f, 9.8f});
        _camera = { 0, 0, 0, 0 };
    }
    int load_content() {
        b0 = create_block(_world, {-100, 0, 200, 8 });
        p  = create_player(_world, {0, -150});
        b1 = create_block(_world, { -16, -300, 32, 32 });
        create_block(_world, { -16, -200, 32, 32 });
        create_block(_world, { 200, -100, 32, 32 });
        create_block(_world, { -16, 100, 32, 32 });
        create_block(_world, { -16, 150, 32, 32 });
        _rope = create_rope_between(_world, p, b1, kult::get<rigidbody>(b1).body->GetWorldCenter());
        
    }

    int update(int dt) {
        _world->Step(1.0f / 60.0f, 4, 3);
        articulation_t& a = kult::get<articulation>(_rope);
        for(auto& e : kult::join<raycast>()) {
            raycast_t& r = kult::get<raycast>(e);
            if(r.is_hit) {
                r.is_hit = false;
                if(a.joint) {
                    _world->DestroyJoint(a.joint);
                }
                _rope = create_rope_between(_world, p, e, b2Vec2{r.point.x, r.point.y});
            }
        }

        b2Vec2 pos = WORLD_TO_SCREEN(kult::get<rigidbody>(p).body->GetPosition());
        _camera.x = pos.x;
        _camera.y = pos.y;
    }

    int draw(SDL_Renderer* renderer) {
        SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
        b2DebugDrawEXT(_world, renderer, _camera);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
        SDL_RenderDrawLine(renderer, 0, _camera.h >> 1, _camera.w, _camera.h >> 1);

        
        int cx = -_camera.x + (_camera.w >> 1);
        int cy = -_camera.y + (_camera.h >> 1);

        b2Vec2 pa = kult::get<rigidbody>(p).body->GetPosition();
        b2Vec2 temp = { _mouse_pos.x, _mouse_pos.y };
        b2Vec2 pb = SCREEN_TO_WORLD(temp);


        pa = WORLD_TO_SCREEN(pa);
        pb = WORLD_TO_SCREEN(pb);



        SDL_RenderDrawLine(renderer, cx + pa.x, cy + pa.y, pb.x, pb.y);
        
        SDL_RenderPresent(renderer);
    }

    int on_click() {
        for(auto& e : kult::join<raycast>()) {
            kult::get<raycast>(e).is_hit = false;
        }
        int cx = -_camera.x + (_camera.w >> 1);
        int cy = -_camera.y + (_camera.h >> 1);

        b2Vec2 pa = kult::get<rigidbody>(p).body->GetPosition();
        b2Vec2 temp = { _mouse_pos.x - cx, _mouse_pos.y - cy };
        b2Vec2 pb = SCREEN_TO_WORLD(temp);

        //_world->RayCast(&_raycast, pa, pb);
        _raycast.RayCast(_world, pa, pb);
        return 0;
    }

    void callbacks(application* app) {
        app->on_mousedown += [&] (char) {
            this->_mousedown = true;
            this->_time_down = SDL_GetTicks();
            return 0;
        };

        app->on_mouseup += [&] (char) {
            this->_mousedown = false;
            int dt = (SDL_GetTicks() - this->_time_down);
            static const int TIMEOUT = 500;
            DEBUG_VALUE_OF(dt);
            if(dt < TIMEOUT) {
                return this->on_click();
            }
            return 0;
        };

        app->on_mousemove += [&] (int x, int y) {
            this->_mouse_pos.x = (float)x;
            this->_mouse_pos.y = (float)y;
            return 0;
        };
    }

};

int main(int, char*[]) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->callbacks(&app);
    return app.start();
}
