#undef KULT_BUILD_TESTS
#include "common/common.hpp"
#include "common/collisions.hpp"
#include "common/components.hpp"
#include "common/Box2DEXT.hpp"

#include <kult.hpp>

#include <Box2D/Box2D.h>


#include <nlohmann/json.hpp>
#include <SDL.h>
#include <core/circular_array.hpp>
#include <util/logstream.hpp>
#include <engine/assets.hpp>
#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <engine/particles.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>

#include <core/vec2.hpp>
//#include "terrain.hpp"

using nlohmann::json;
using kult::entity;


using namespace wee;


float sigmoid(float x, float k) {
    return (x - x * k) / (k - std::abs(x) * 2 * k + 1);
}






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
/*
template <typename T>
struct particle_collision_emitter {
    static kult::type create(b2World* world, kult::entity parent, const vec2& offset, float radius, std::function<voi) {
        kult::type self = kult::entity();

        kult::add<nested>(id).parent = parent;
        kult::get<nested>(id).offset = offset;

        b2BodyDef bd;
        bd.type = b2_staticBody;
        kult::add<rigidbody>(id).body = world->CreateBody(&bd);



        b2FixtureDef fd;
        fd.isSensor = true;
        fd.shape = &shape;


    }
};*/

struct player {
    static kult::type create(b2World* world, const vec2& at) {

        kult::type self = kult::entity();

        kult::add<input>(self).mouse_is_down = false;
        kult::get<input>(self).is_jumping = true;

        /*kult::add<visual>(self).texture = assets<SDL_Texture>::instance().load(
            "@animal", 
            ::as_lvalue(
                std::ifstream(
                    wee::get_resource_path("") + "assets/img/sloth.png"
                )
            )
        );

        SDL_QueryTexture(kult::get<visual>(self).texture,
            NULL,
            NULL,
            &kult::get<visual>(self).src.w,
            &kult::get<visual>(self).src.h
        );*/



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
            fd.shape = &shape;
            fd.userData = (void*)self;

            kult::add<collider>(self).fixture = kult::get<rigidbody>(self).body->CreateFixture(&fd);
            kult::get<collider>(self).enter = [&] (const collision& c) {
                kult::get<input>(c.self).is_jumping = false;
                kult::get<input>(c.self).N = c.normal;

            };
            kult::get<collider>(self).leave = [&] (const collision& c) {
                kult::get<input>(c.self).is_jumping = true;
            };
            
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

        float r = randf(75.0f);

        constexpr const float to_rad = static_cast<float>(M_PI) / 180.0f;
        vec2 direction = vec2::from_angle(to_rad * 10.0f); // 15deg decline
        vec2 normal = vec2::normal_of(direction);
        
        for(int i=0; i < n; i++) {

            float x = direction.x * i * step;
            float y = direction.y * i * step;//normal.y;// * i * step;//;

            float ry = std::cos(2.0f * static_cast<float>(M_PI) * ((float)i / n)) * r;

            x += normal.x * ry;
            y += normal.y * ry;


            DEBUG_LOG("{},{}", x, y);

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

struct pstate {
    float vx, vy;
    float scale;
    float rotation;


    static pstate _; 
};

pstate pstate::_ = { 0.0f, 0.0f };

struct particle_helper {
    static void spray(particles<pstate>* em, const b2Vec2& pos, const b2Vec2& vel, const vec2& n) {
        DEBUG_METHOD();

        float vlen_sq = vel.LengthSquared() / 8.0f;

        //for(int i=0; i < 10; i++) {
            particles<pstate>::particle res;
            res.x = pos.x;
            res.y = pos.y;
            res.t = 0;
            res.ttl = 1000 + (int)randf(2000.0f);
            res.color = SDL_ColorPresetEXT::White;
            float fs = randf();
            res.state = { 
                n.x * vlen_sq * fs, 
                n.y * vlen_sq * fs, 
                1.0f, 
                randf() * 2.0f * M_PI 
            };
            

            em->emit(res);
        //}
    }
};

struct game : applet {
    SDL_Rect camera_;
    float zoom_;
    b2World* world_;
    kult::type _player;

    particles<pstate>* _particles;

    std::vector<kult::type> chunks;

    int _airtime = 0;

    constexpr static const int NUM_CHUNKS = 2;

    void set_callbacks(application* app) {
        app->on_mousemove += [&] (int x, int y) {
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
        auto context = SDL_GL_GetCurrentContext();

        std::string pt = wee::get_resource_path("") + "assets/img/smoke.png";
        DEBUG_VALUE_OF(pt);
        assets<SDL_Texture>::instance().load("@dirt", ::as_lvalue(std::ifstream(pt)));

        _particles = new particles<pstate>(32, [&] (particles<pstate>::particle& p, int dt) {

            p.state.vy += 1.f / dt * 9.8f;

            p.x += p.state.vx;
            p.y += p.state.vy;

            p.state.vx *= 0.9f;
            p.state.vy *= 0.9f;

            float ip = (float)p.t / p.ttl;

            p.state.scale = 1.0f + (3 * ip);
            p.state.rotation = -dt * ip * 8.0f * M_PI;

            p.t += dt;

            p.color.a = (int)(1.0f - ip * 255.f);
        });
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

                b2Body* body = kult::get<rigidbody>(id).body;
                const input_t& ip = kult::get<input>(id);

                if(ip.mouse_is_down) {
                    body->ApplyForceToCenter(b2Vec2(.5f, 9.0f), true);
                }

                if(ip.is_jumping) {
                    _airtime += dt;
                } else {
                    particle_helper::spray(_particles, WORLD_TO_SCREEN(body->GetWorldCenter()), body->GetLinearVelocity(), ip.N);
                }
            }
            player::limit_velocity(_player);
        }

        _particles->update(dt);

        return 0; 
    }
    virtual int draw(SDL_Renderer* renderer) { 
        
        SDL_RenderGetLogicalSize(renderer, &camera_.w, &camera_.h);
        {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
            SDL_RenderClear(renderer);

            b2DebugDrawEXT(world_, renderer, camera_);
                

            int cx = -camera_.x + (camera_.w >> 1);
            int cy = -camera_.y + (camera_.h >> 1);
            for(auto& id : kult::join<transform, visual>()) {
                const visual_t& vis = kult::get<visual>(id);
                const transform_t& tx = kult::get<transform>(id);
                SDL_Rect dst = {
                    cx + static_cast<int>(tx.p.x) - (vis.src.w >> 1),
                    cy + static_cast<int>(tx.p.y) - (vis.src.h >> 1),
                    vis.src.w,
                    vis.src.h
                };

                SDL_RenderCopyEx(renderer, 
                    vis.texture,
                    &vis.src,
                    &dst,
                    tx.t,
                    NULL,
                    SDL_FLIP_NONE);

            }

            _particles->draw([&] (const particles<pstate>::container_type& particles) {
                SDL_Texture* tex = assets<SDL_Texture>::instance().get("@dirt");
                int w, h;
                SDL_QueryTexture(tex, NULL, NULL, &w, &h);

                SDL_Rect src = { 0, 0, w, h };

                int cx = -camera_.x + (camera_.w >> 1);
                int cy = -camera_.y + (camera_.h >> 1);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                for(size_t i=0; i < particles.size(); ++i) {
                    auto& ref = particles[i];

                    int ww = (int)(w * ref.state.scale);
                    int hh = (int)(h * ref.state.scale);

                    SDL_Rect dst = { 
                        cx + static_cast<int>((ref.x - w / 2)), 
                        cy + static_cast<int>((ref.y - h / 2)), 
                        ww,
                        hh
                    };
                    //SDL_SetRenderDrawColor(renderer, ref.color.r, ref.color.g, ref.color.b, ref.color.a);
                    SDL_SetTextureColorMod(tex, ref.color.r, ref.color.g, ref.color.b);
                    SDL_SetTextureAlphaMod(tex, ref.color.a);

                    
                    SDL_RenderCopyEx(renderer, tex, &src, &dst, ref.state.rotation, NULL, SDL_FLIP_NONE);
                }
            });

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

