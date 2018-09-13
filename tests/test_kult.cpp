#undef KULT_BUILD_TESTS
#include <kult.hpp>

#include <Box2D/Box2D.h>

#define DEFAULT_COLLIDER_CALLBACK [] (kult::type, kult::type, const vec2&) {}

#include <nlohmann/json.hpp>
#include <SDL.h>
#include <core/circular_array.hpp>
#include <util/logstream.hpp>
#include <engine/assets.hpp>
#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>

#include "vec2.h"
#include "Box2DEXT.hpp"
#include "terrain.hpp"

using namespace wee;

using nlohmann::json;
using kult::entity;

float sigmoid(float x, float k) {
    return (x - x * k) / (k - std::abs(x) * 2 * k + 1);
}

typedef struct {
    b2Fixture* fixture = NULL;
    std::function<void(kult::type, kult::type, const vec2&)> enter = DEFAULT_COLLIDER_CALLBACK;
    std::function<void(kult::type, kult::type, const vec2&)> leave = DEFAULT_COLLIDER_CALLBACK;
} collider_t; 

std::ostream& operator << (std::ostream& os, const collider_t& c) {
    return os;
}

typedef struct {
    b2Body* body;
} rigidbody_t;

std::ostream& operator << (std::ostream& os, const rigidbody_t& rb) {
    return os << rb.body;
}

typedef struct {
    vec2 p;
    float t;
} transform_t;

std::ostream& operator << (std::ostream& os, const transform_t& tx) {
    return os;
}

typedef struct {
    kult::type parent;
    vec2 offset;
} nested_t;

std::ostream& operator << (std::ostream& os, const nested_t& tx) {
    return os;
}

typedef struct {
    SDL_Texture* tex;
    SDL_Rect     src;
    SDL_Rect     dst;
    SDL_Color    color;
} visual_t;

std::ostream& operator << (std::ostream& os, const visual_t& tx) {
    return os;
}

typedef struct {
    float* dst;
    std::function<float(float, float, float)> easing;
} tween_t;

typedef struct {
    int time;
    int timeout;
} timeout_t;

typedef struct {
    //vec2 last;
    //float max_x;
    //float len;
} terrain_t;

std::ostream& operator << (std::ostream& os, const terrain_t& t) {
    return os;
}


using collider  = kult::component<1 << 0, collider_t>;
using rigidbody = kult::component<1 << 1, rigidbody_t>;
using nested    = kult::component<1 << 2, nested_t>;
using transform = kult::component<1 << 3, transform_t>;
using visual    = kult::component<1 << 4, visual_t>;
using terrain   = kult::component<1 << 5, terrain_t>;


class collisions : public b2ContactListener {
public:
    collisions() {}
    virtual void BeginContact(b2Contact* contact) {

        const b2Fixture* fA = contact->GetFixtureA();
        const b2Fixture* fB = contact->GetFixtureB();
        
        auto objA = (kult::type)fA->GetUserData(); 
        auto objB = (kult::type)fB->GetUserData();

        b2WorldManifold man;
        contact->GetWorldManifold(&man);
        if(kult::has<collider>(objA)) {
            kult::get<collider>(objA).enter(objA, objB, {man.normal.x, man.normal.y});
        }
        
        if(kult::has<collider>(objB)) {
            kult::get<collider>(objB).enter(objB, objA, {man.normal.x, man.normal.y});
        }
        
        
    }
    virtual void EndContact(b2Contact*) {
    }
};

enum class collision_filter : uint16_t {
    environment = 1 << 0,
    player = 1 << 1,
    collectable = 1 << 2,
    any = 0xffff
};


#define HILLS_PER_CHUNK         2
#define HILLS_PIXELSTEP         10
#define HILLS_WIDTH             640
#define HILLS_VERTEX_COUNT      HILLS_PER_CHUNK * HILLS_WIDTH / HILLS_PIXELSTEP


struct terrain_chunk {
    static kult::type create(b2World* world) {
        kult::type self = kult::entity();

        {
            b2BodyDef bd;
            bd.type = b2_staticBody;
            kult::add<rigidbody>(self).body = world->CreateBody(&bd);
        }

        {
            b2ChainShape shape;
            b2FixtureDef fd;
            fd.shape =  &shape;
            kult::add<collider>(self).fixture = kult::get<rigidbody>(self).body->CreateFixture(&fd);
        }

        kult::add<terrain>(self);
    }

    static void reset(kult::type self, const vec2& startPosition) {

        static const int n = 10;
        static const vec2 step = {10.0f, 0.0f};

        std::vector<b2Vec2> vertices;

        for(int i=0; i < n; i++) {
            vec2 pos = startPosition + i * step;
            vertices[i] = { SCREEN_TO_WORLD(pos.x), SCREEN_TO_WORLD(pos.y) };
            kult::get<terrain>(self).last = pos;
        }

        b2ChainShape* shape = dynamic_cast<b2ChainShape*>(kult::get<collider>(self).fixture->GetShape());
        shape->CreateChain(&vertices[0], n);
    }
};

struct player {
    static kult::type create(b2World* world, const vec2& at) {

        kult::type self = kult::entity();
        
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody; 
            kult::add<rigidbody>(self) = { world->CreateBody(&bd) };
        }

        kult::add<transform>(self) = {at, 0.0f};

        {
            b2CircleShape shape;
            shape.m_p.Set(0.0f, 0.0f);
            shape.m_radius = SCREEN_TO_WORLD(10.0f);
            b2FixtureDef fd;
            //fd.filter.categoryBits = (uint16_t)collision_filter::player;
            //fd.filter.maskBits = (uint16_t)collision_filter::any;
            fd.density = 1.0f;
            fd.restitution = 0.0f;
            fd.shape = &shape;
            fd.userData = (void*)self;

            kult::add<collider>(self).fixture = kult::get<rigidbody>(self).body->CreateFixture(&fd);
            kult::get<collider>(self).enter = DEFAULT_COLLIDER_CALLBACK;
            
        }
        return self;
    }

    static void limit_velocity(kult::type _player) {
        b2Vec2 vel = kult::get<rigidbody>(_player).body->GetLinearVelocity();
        const float MIN_VELOCITY_X = 3.0f;
        const float MAX_VELOCITY_X = 18.0f;
        const float MIN_VELOCITY_Y = -40.0f;
        const float MAX_VELOCITY_Y = 8.0f;
        vel.x = std::max(std::min(vel.x, MAX_VELOCITY_X), MIN_VELOCITY_X);
        vel.y = std::max(std::min(vel.y, MAX_VELOCITY_Y), MIN_VELOCITY_Y);
        kult::get<rigidbody>(_player).body->SetLinearVelocity(vel);
    }
};

using namespace wee;

auto copy_transform_to_physics = [] () {
    for(auto& e : kult::join<transform, rigidbody>()) {
        const vec2& p = kult::get<transform>(e).p;
        float r = kult::get<transform>(e).t;
        kult::get<rigidbody>(e).body->SetTransform({ 
                SCREEN_TO_WORLD(p.x), 
                SCREEN_TO_WORLD(p.y) 
            }, r
        );
    }
};

auto copy_physics_to_transform = [] () {
    for(auto& e : kult::join<transform, rigidbody>()) {
        const b2Transform b2t = kult::get<rigidbody>(e).body->GetTransform();
        const b2Vec2& vec = b2t.p;
        
        kult::get<transform>(e).p.x = WORLD_TO_SCREEN(vec.x);
        kult::get<transform>(e).p.y = WORLD_TO_SCREEN(vec.y);
        kult::get<transform>(e).t   = kult::get<rigidbody>(e).body->GetAngle();
    }
};

#define MAX_CHUNKS  3

struct game : applet {
    SDL_Rect camera_;
    float zoom_;
    b2World* world_;
    kult::type _player;
    kult::type _chunks[MAX_CHUNKS];

    circular_array<kult::type> _active_chunks;

    virtual int load_content() { 
        world_ = new b2World({0.0f, 9.8f});

        world_->SetContactListener(new collisions);

        _player = player::create(world_, { 320.0f, -100 });



        camera_ = {
            0, 240, 0, 0
        };
        return 0; 
    }
    virtual int update(int dt) { 
        copy_transform_to_physics();
        world_->Step(1.0f / 60.0f, 6, 3);
        copy_physics_to_transform();

        const transform_t& tx = kult::get<transform>(_player);
        camera_.x = tx.p.x;
        camera_.y = tx.p.y;

        player::limit_velocity(_player);

        return 0; 
    }
    virtual int draw(SDL_Renderer* renderer) { 
        
        SDL_RenderGetLogicalSize(renderer, &camera_.w, &camera_.h);
        {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);//IndianRed);
            SDL_RenderClear(renderer);


            b2DebugDrawEXT(world_, renderer, camera_);


            SDL_RenderPresent(renderer);
        }
        return 0; 
    }
};

int main(int, char* []) {

    applet* let = new game;
    application app(let);


    /*while(1) {
        simulation->Step(fDt, 8, 3, 4);

    }*/
    return app.start();
}

