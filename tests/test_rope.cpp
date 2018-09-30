#include "common/common.hpp"
#include "common/components.hpp"
#include "common/Box2DEXT.hpp"
#include <util/logstream.hpp>
#include <kult.hpp>
#include <Box2D/Box2D.h>

using namespace wee;

typedef kult::type entity_type;

struct camera {
    SDL_Rect rect;
};

entity_type create_rope_between(b2World* world, entity_type a, entity_type b) {
    b2RopeJointDef def;
    def.userData = static_cast<void*>(nullptr);
    def.bodyA = kult::get<rigidbody>(a).body; 
    def.bodyB = kult::get<rigidbody>(b).body; 
    def.collideConnected = false;
    // center of player
    def.localAnchorA = b2Vec2 {0.0f, 0.0f};
    // raycast the world here
    def.localAnchorB = b2Vec2 {0.0f, 0.0f};
    def.maxLength = (def.bodyA->GetPosition() - def.bodyB->GetPosition()).Length();//SCREEN_TO_WORLD(0.0f);

    world->CreateJoint(&def);
}

void raycast(const b2Vec2& a, const b2Vec2& b, b2Vec2* at, b2Vec2* n) {
 
}

entity_type create_player(b2World* world, const SDL_Point& at) {
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
        fd.filter.categoryBits = !0;//(uint16_t)collision_filter::player;
        fd.filter.maskBits = !0;//(uint16_t)collision_filter::any;
        fd.density = 1.0f;
        fd.restitution = 0.0f;
        fd.shape = &shape;
        fd.userData = nullptr;//(void*)self;

        fixture = body->CreateFixture(&fd);
    }
    entity_type self = kult::entity();
    kult::add<rigidbody>(self).body = body;
    kult::add<collider>(self).fixture = fixture;
    return self;
}

entity_type create_block(b2World* world, const SDL_Rect& r) {
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
        def.filter.categoryBits = !0;
        def.filter.maskBits = !0;
        
        fixture = body->CreateFixture(&def);
    }

    entity_type self = kult::entity();
    kult::add<rigidbody>(self).body = body;
    kult::add<collider>(self).fixture = fixture;
    return self;
}


class game : public applet {
    b2World* _world;
    bool _mousedown = false;
    int _time_down = 0;
    SDL_Rect _camera;
public:
    game() {
        _world = new b2World({0.0f, 9.8f});
        _camera = { 0, 0, 0, 0 };
    }
    int load_content() {
        entity_type b0 = create_block(_world, {-100, 0, 200, 10 });
        entity_type p = create_player(_world, {0, -100});
        entity_type b1 = create_block(_world, { -16, -200, 32, 32 });
        create_rope_between(_world, p, b1);
        
    }

    int update(int dt) {
        _world->Step(1.0f / 60.0f, 4, 3);
    }

    int draw(SDL_Renderer* renderer) {
        SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
        b2DebugDrawEXT(_world, renderer, _camera);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
        SDL_RenderDrawLine(renderer, 0, _camera.h >> 1, _camera.w, _camera.h >> 1);
        SDL_RenderPresent(renderer);
    }

    int on_click() {
        DEBUG_METHOD();
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
            static const int TIMEOUT = 100;
            DEBUG_VALUE_OF(dt);
            if(dt < TIMEOUT) {
                return this->on_click();
            }
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
