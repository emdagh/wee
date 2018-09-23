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

#include <core/vec2.hpp>
#include "Box2DEXT.hpp"
#include "terrain.hpp"

using nlohmann::json;
using kult::entity;

namespace wee {
    void to_json(json& j, const vec2& v) {
        j = {
            { "x" , v.x },
            { "y" , v.y }
        };
    }

    void from_json(const json& j, vec2& v) {
        v.x = j["x"];
        v.y = j["y"];
    }
}

using namespace wee;


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
    vec2 last;
    kult::type next;
} terrain_t;

std::ostream& operator << (std::ostream& os, const terrain_t& t) {
    json j = {
        {"last", t.last},
        {"next", t.next}
    };
    return os << j;
}

typedef struct {
    bool mouse_is_down;
} input_t;

std::ostream& operator << (std::ostream& os, const input_t& t) {
    return os << t.mouse_is_down;
}

using collider  = kult::component<1 << 0, collider_t>;
using rigidbody = kult::component<1 << 1, rigidbody_t>;
using nested    = kult::component<1 << 2, nested_t>;
using transform = kult::component<1 << 3, transform_t>;
using visual    = kult::component<1 << 4, visual_t>;
using terrain   = kult::component<1 << 5, terrain_t>;
using input     = kult::component<1 << 7, input_t>;


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

struct player {
    static kult::type create(b2World* world, const vec2& at) {

        kult::type self = kult::entity();

        kult::add<input>(self).mouse_is_down = false;

        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody; 
            bd.linearDamping = 0.05f;
            kult::add<rigidbody>(self) = { world->CreateBody(&bd) };
        }

        kult::add<transform>(self) = {at, 0.0f};

        {
            b2CircleShape shape;
            shape.m_p.Set(0.0f, 0.0f);
            shape.m_radius = SCREEN_TO_WORLD(20.0f);
            b2FixtureDef fd;
            //fd.filter.categoryBits = (uint16_t)collision_filter::player;
            //fd.filter.maskBits = (uint16_t)collision_filter::any;
            fd.density = 1.0f;
            fd.restitution = 0.0f;
            fd.friction = 0.0f;
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
        const float MAX_VELOCITY_X = 25.0f;
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


struct terrain_chunk {

    constexpr static const int n = 64;
    constexpr static const float step = 10.0f;

    static kult::type create(b2World* world, kult::type prev) {
        kult::type res = kult::entity();
        

        b2BodyDef bd;
        b2Body* body = world->CreateBody(&bd);
        
        kult::add<collider>(res);
        kult::add<terrain>(res);
        kult::add<rigidbody>(res).body = body;

        kult::get<terrain>(res).last = { -1024.f, 0.f };
        
        if(prev != kult::none())
            kult::get<terrain>(prev).next = res;

        return res;
    }

    static void b2DestroyAllFixtures(b2Body* body) {
        for(auto* f=body->GetFixtureList(); f; ) {
            auto* ptr = f;
            f = f->GetNext();
            body->DestroyFixture(ptr);
        }
    }

    static void reset(kult::type self,  const vec2& start) {


        auto* body = kult::get<rigidbody>(self).body;
        
        b2DestroyAllFixtures(body);

        std::vector<b2Vec2> vertices(n);
        b2Vec2 finish;

        float r = randf(150.f);

        for(int i=0; i < n; i++) {

            float x = i * step;
            float y = std::cos(2 * M_PI * ((float)i / n)) * r; 

            b2Vec2& vec = vertices[i];
            vec.x = start.x + x;
            vec.y = start.y + y - r;

            finish = { vec.x, vec.y };
        }

        kult::get<terrain>(self).last = { finish.x, finish.y };

        std::transform(vertices.begin(), vertices.end(), vertices.begin(), [&] (const b2Vec2& x) {
            return SCREEN_TO_WORLD(x);
        });
        
        b2ChainShape shape;
        shape.CreateChain(&vertices[0], vertices.size());

        b2FixtureDef fdef;
        fdef.shape = &shape;

        kult::get<collider>(self).fixture = body->CreateFixture(&fdef);
    }

};

struct game : applet {
    SDL_Rect camera_;
    float zoom_;
    b2World* world_;
    kult::type _player;

    std::vector<kult::type> chunks;

    constexpr static const int NUM_CHUNKS = 2;

    void set_callbacks(application* app) {
        app->on_mousemove += [&] (int x, int y) {
            //DEBUG_LOG("mouse={},{}", x, y);
            return 0;
        };

        app->on_mousedown += [&] (char) {
            DEBUG_LOG("Mouse down");
            kult::get<input>(_player).mouse_is_down = true;
            return 0;
        };

        app->on_mouseup += [&] (char) {
            DEBUG_LOG("Mouse up");
            kult::get<input>(_player).mouse_is_down = false;
            return 0;
        };

    }

    virtual int load_content() { 
        world_ = new b2World({0.0f, 9.8f});

        world_->SetContactListener(new collisions);

        _player = player::create(world_, { 5.0f, -150 });

        vec2 lastPosition = { 0.0f, 0.0f };
        kult::type prev = kult::none();
        for(int i=0; i < NUM_CHUNKS; i++) {
            kult::type id = terrain_chunk::create(world_, prev);
            prev = id;
            chunks.push_back(id);
        }
        kult::get<terrain>(chunks.back()).next = chunks.front();

        camera_ = {
            0, 240, 0, 0
        };
        return 0; 
    }
    virtual int update(int dt) {
        static float t = 0.f;
        t += (float)dt;
        copy_transform_to_physics();
        world_->Step(1.0f / 60.0f, 6, 3);
        copy_physics_to_transform();

        const transform_t& tx = kult::get<transform>(_player);
        camera_.x = tx.p.x;
        camera_.y = tx.p.y;

        {
            for(auto& id : kult::join<terrain>()) {
                terrain_t& t = kult::get<terrain>(id);

                if((camera_.x - camera_.w / 2) > t.last.x) {
                    terrain_chunk::reset(id, kult::get<terrain>(t.next).last);
                }
            }
        }

        {
            for(auto& id : kult::join<input, rigidbody>()) {

                if(kult::get<input>(id).mouse_is_down) {
                    kult::get<rigidbody>(id).body->ApplyForceToCenter(b2Vec2(1.5f, 9.0f), true);
                }
            }
            player::limit_velocity(_player);
        }

        return 0; 
    }
    virtual int draw(SDL_Renderer* renderer) { 
        
        SDL_RenderGetLogicalSize(renderer, &camera_.w, &camera_.h);
        {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
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
    ((game*)let)->set_callbacks(&app);
    return app.start();
}

