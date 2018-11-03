#undef KULT_BUILD_TESTS

#include <engine/ecs.hpp>
#include <engine/b2Adapters.hpp>

#include "common/Box2DEXT.hpp"
#include "common/camera.hpp"
#include "common/components.hpp"




#include <nlohmann/json.hpp>
#include <SDL.h>
#include <core/circular_array.hpp>
#include <core/logstream.hpp>
#include <engine/assets.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/particles.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>

#include <core/vec2.hpp>
//#include "terrain.hpp"

using nlohmann::json;
using kult::entity;


using namespace wee;

struct pstate {
    float vx, vy;
    float scale;
    float rotation;


    static pstate _; 
};

pstate pstate::_ = { 0.0f, 0.0f, 0.0f, 0.0f };

struct particle_helper {
    static void spray(particles<pstate>* em, const b2Vec2& pos, const b2Vec2& vel, const vec2& n) {
        DEBUG_METHOD();
        DEBUG_VALUE_OF(em);

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
                randf() * 2.0f * (float)M_PI 
            };
            

            em->emit(res);
        //}
    }
};


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
        kult::add<physics>(id).body = world->CreateBody(&bd);



        b2FixtureDef fd;
        fd.isSensor = true;
        fd.shape = &shape;


    }
};*/

struct avatar {
    static kult::type create(b2World* world, const vec2& at, wee::particles<pstate>* _particles) {
        DEBUG_VALUE_OF(_particles);
        kult::type self = kult::entity();
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody; 
            bd.linearDamping = 0.05f;
            kult::add<physics>(self) = { world->CreateBody(&bd) };
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

            kult::get<physics>(self).body->CreateFixture(&fd);
            kult::get<physics>(self).on_collision_enter = [_particles] (const collision& c) {
                b2Body* body = kult::get<physics>(c.self).body;

                /*
                 * TODO: this sort of merits a "particles" component. What would it look like though?
                 * If I just link to a singleton (manager) type thing; it's not good practice.
                 * But it seems like the only sensible way to go...
                 * 
                 * Option 2 would be to have the particles as an argument to the player::create function
                 * and having the lambda of the collider capture it. 
                 *
                 * I prefer option 2 because a singleton is usually a canary-in-the-colemine for bad design.
                 */
                DEBUG_VALUE_OF(c.n);//vec2f::length(c.normal));
                particle_helper::spray(_particles, WORLD_TO_SCREEN(body->GetWorldCenter()), body->GetLinearVelocity(), c.n);

                //kult::get<input>(c.self).is_jumping = false;
                //kult::get<input>(c.self).N = c.normal;

            };

            SDL_Texture* texture = nullptr;
            {
                auto is = std::ifstream(wee::get_resource_path("assets/img/Assets/PNG/Players/Player Red") + "playerRed_roll.png", std::ios::binary);
                assert(is.is_open());
                texture = assets<SDL_Texture>::instance().load("@player",is);

                kult::add<visual>(self);
                auto& v = kult::get<visual>(self);
                v.texture = texture;
                SDL_QueryTexture(texture, NULL, NULL, &v.src.w, &v.src.h);
                v.src.x = v.src.y = 0;

                auto& n = kult::add<nested>(self);
                n.offset.x = -v.src.w / 2;
                n.offset.y = -v.src.h / 2;

            }
            
        }
        return self;
    }

    static void limit_velocity(kult::type _player) {
        b2Vec2 vel = kult::get<physics>(_player).body->GetLinearVelocity();
        const float MIN_VELOCITY_X = 3.0f;
        const float MAX_VELOCITY_X = 25.0f;
        const float MIN_VELOCITY_Y = -40.0f;
        const float MAX_VELOCITY_Y = 8.0f;
        vel.x = std::max(std::min(vel.x, MAX_VELOCITY_X), MIN_VELOCITY_X);
        vel.y = std::max(std::min(vel.y, MAX_VELOCITY_Y), MIN_VELOCITY_Y);
        kult::get<physics>(_player).body->SetLinearVelocity(vel);
    }
};


using namespace wee;

auto copy_transform_to_physics = [] () {
    for(auto& e : kult::join<transform, physics>()) {
        const vec2& p = kult::get<transform>(e).position;
        float r = kult::get<transform>(e).rotation * M_PI / 180.0f;
        kult::get<physics>(e).body->SetTransform({ 
                SCREEN_TO_WORLD(p.x), 
                SCREEN_TO_WORLD(p.y) 
            }, r
        );
    }
};

auto copy_physics_to_transform = [] () {
    for(auto& e : kult::join<transform, physics>()) {
        const b2Transform b2t = kult::get<physics>(e).body->GetTransform();
        const b2Vec2& vec = b2t.p;
        
        kult::get<transform>(e).position.x = WORLD_TO_SCREEN(vec.x);
        kult::get<transform>(e).position.y = WORLD_TO_SCREEN(vec.y);
        kult::get<transform>(e).rotation   = kult::get<physics>(e).body->GetAngle() *  180.0f / M_PI;
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
        
        kult::add<terrain>(res);
        kult::add<physics>(res).body = body;

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


        auto* body = kult::get<physics>(self).body;
        
        b2DestroyAllFixtures(body);

        std::vector<b2Vec2> vertices(n);
        b2Vec2 finish;

        float r = randf(75.0f);

        constexpr const float to_rad = static_cast<float>(M_PI) / 180.0f;
        vec2 direction = vec2::from_angle(to_rad * 15.0f); 
        vec2 normal = vec2::normal_of(direction);
        
        for(int i=0; i < n; i++) {

            float x = direction.x * i * step;
            float y = direction.y * i * step;//normal.y;// * i * step;//;

            float ry = std::cos(2.0f * static_cast<float>(M_PI) * ((float)i / n)) * r;

            x += normal.x * ry;
            y += normal.y * ry;



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

        body->CreateFixture(&fdef);
    }

};


struct game : applet {
    SDL_Rect viewport_;
    float zoom_;
    b2World* world_;
    kult::type _player;
    b2DebugDrawImpl _debugdraw;

    particles<pstate>* _particles;
    camera _cam;

    std::vector<kult::type> chunks;

    int _airtime = 0;

    bool mouse_is_down;

    constexpr static const int NUM_CHUNKS = 2;

    void set_callbacks(application* app) {
        app->on_mousemove += [&] (int, int) {
            return 0;
        };

        app->on_mousedown += [&] (char) {
            DEBUG_LOG("Mouse down");
            this->mouse_is_down = true;
            return 0;
        };

        app->on_mouseup += [&] (char) {
            DEBUG_LOG("Mouse up");
            this->mouse_is_down = false;
            return 0;
        };

    }

	bool file_exists(const std::string& path) {
		std::ifstream is(path);
		if (is.is_open()) {
			is.close();
			return true;
		}
		return false;
	}

    virtual int load_content() {
		
        _debugdraw.SetFlags(
            b2Draw::e_shapeBit          | //= 0x0001, 
            b2Draw::e_jointBit          | //= 0x0002, 
            //b2Draw::e_aabbBit           | //= 0x0004, 
            //b2Draw::e_pairBit           | //= 0x0008, 
            b2Draw::e_centerOfMassBit   | //= 0x0010, 
            //e_particleBit// = 0x0020 
            0
        ) ;
        //auto context = SDL_GL_GetCurrentContext();
		try {
			
			std::string pt = wee::get_resource_path("assets/img") + "smoke.png";
			DEBUG_VALUE_OF(pt);
			assets<SDL_Texture>::instance().load("@dirt", ::as_lvalue(std::ifstream(pt, std::ios::binary)));
		}
		catch (const std::exception& err) {
			DEBUG_LOG(err.what());
		}

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

        world_->SetContactListener(new b2ContactListenerImpl);
        world_->SetDebugDraw(&_debugdraw);

        DEBUG_VALUE_OF(_particles);
        _player = avatar::create(world_, { 5.0f, -150 }, _particles);

        kult::type prev = kult::none();
        for(int i=0; i < NUM_CHUNKS; i++) {
            kult::type id = terrain_chunk::create(world_, prev);
            prev = id;
            chunks.push_back(id);
        }
        kult::get<terrain>(chunks.back()).next = chunks.front();

        viewport_ = {
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
        /*viewport_.x = tx.p.x;
        viewport_.y = tx.p.y;*/
        _cam.set_position(tx.position.x, tx.position.y);
        _cam.update(dt);
        _debugdraw.SetCameraTransform(_cam.get_transform());

        {
            for(auto& id : kult::join<terrain>()) {
                terrain_t& t = kult::get<terrain>(id);

                if((tx.position.x - viewport_.w / 2) > t.last.x) {
                    terrain_chunk::reset(id, kult::get<terrain>(t.next).last);
                }
            }
        }

        {


                auto* body = kult::get<physics>(_player).body;
                if(this->mouse_is_down) {
                    body->ApplyForceToCenter(b2Vec2(.5f, 9.0f), true);
                }

                /*if(ip.is_jumping) {
                    _airtime += dt;
                } else {
                }*/
            avatar::limit_velocity(_player);
        }

        _particles->update(dt);

        return 0; 
    }
    virtual int draw(SDL_Renderer* renderer) { 
        
        _debugdraw.SetRenderer(renderer);
        SDL_RenderGetLogicalSize(renderer, &viewport_.w, &viewport_.h);
        _cam.set_viewport(viewport_.w, viewport_.h);
        {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
            SDL_RenderClear(renderer);
            
            world_->DrawDebugData();
            
            _particles->draw([&] (const particles<pstate>::container_type& particles) {
                SDL_Texture* tex = assets<SDL_Texture>::instance().get("@dirt");
                int w, h;
                SDL_QueryTexture(tex, NULL, NULL, &w, &h);

                SDL_Rect src = { 0, 0, w, h };

                int cx = -viewport_.x + (viewport_.w >> 1);
                int cy = -viewport_.y + (viewport_.h >> 1);
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

            for(const auto& e : kult::join<transform, visual, nested>()) {
                const visual_t& v = kult::get<visual>(e);
                const transform_t& t = kult::get<transform>(e);
                const nested_t& n = kult::get<nested>(e);

                vec3 position = { 
                    t.position.x + n.offset.x,
                    t.position.y + n.offset.y,
                    0.0f
                };

                vec3 positionCS = vec3::transform(position, _cam.get_transform());


                SDL_Rect dst = {
                    (int)(positionCS.x + 0.5f), //cx + (int)(t.p.x + v.offset.x + .5f), 
                    (int)(positionCS.y + 0.5f), //cy + (int)(t.p.y + v.offset.y + .5f),
                    v.src.w, 
                    v.src.h
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


            SDL_RenderPresent(renderer);

        }
        return 0; 
    }
};
#undef main
int main(int, char* []) {

    applet* let = new game;
    application app(let);
    ((game*)let)->set_callbacks(&app);
    return app.start();
}

