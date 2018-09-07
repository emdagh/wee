#undef KULT_BUILD_TESTS
#include <kult.hpp>
#include <nlohmann/json.hpp>
#include <Box2D/Box2D.h>
#include <SDL.h>
#include <core/circular_array.hpp>
#include <util/logstream.hpp>

using namespace wee;

#ifndef PTM_RATIO
#define PTM_RATIO   (40.f)
#endif

#define WORLD_TO_SCREEN(x)  ((x) * (PTM_RATIO))
#define SCREEN_TO_WORLD(x)  ((x) / (PTM_RATIO))

using nlohmann::json;
using kult::entity;

struct vec2 {
    float x, y;

    vec2 operator - (const vec2& other) const {
        vec2 copy(*this);
        copy -= other;
        return copy;
    }

    vec2& operator -= (const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    static vec2 from_angle(float rad) {
        return { std::cos(rad), std::sin(rad) };
    }

    static float to_angle(const vec2& a) {
        return std::atan2(a.x, -a.y);
    }

    static float dot(const vec2& a, const vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    static float length(const vec2& a) {
        return std::sqrt(dot(a, a));
    }

    static vec2 normalize(const vec2& a) {
        float len_r = 1.0f / length(a);
        return { a.x * len_r, a.y * len_r };
    }

    static vec2 normal_of(const vec2& a) { // normal is to the left of direction
        vec2 n = normalize(a);
        return { -n.y, n.x };
    }
};

std::ostream& operator << (std::ostream& os, const vec2& vec) {
    return os << vec.x << ", " << vec.y;
}

std::ostream& operator << (std::ostream& os, const b2Transform& tx) {
    return os << tx.p.x << ", " << tx.p.y;
}

typedef struct {
    b2Body* body_;
} rigidbody_t;

std::ostream& operator << (std::ostream& os, const rigidbody_t& rb) {
    return os << rb.body_;
}

typedef struct {
    b2Fixture* fixture_;
    std::function<void(entity*, entity*)> enter;
    std::function<void(entity*, entity*)> leave;
} collider_t;


std::ostream& operator << (std::ostream& os, const collider_t& c) {
    return os << c.fixture_;
}

kult::component<1 << 0, rigidbody_t> rigidbody;
kult::component<1 << 1, collider_t> collider;


class collisions : public b2ContactListener {
public:
    collisions() {}
    virtual void BeginContact(b2Contact*) {
    }
    virtual void EndContact(b2Contact*) {
    }
};

enum class collision_filter : uint16_t {
    environment = 1 << 0,
    any = 0xffff
};


float randf(float scale_) {
    return scale_ * (static_cast<float>(rand()) / RAND_MAX);
}


void fractal(const vec2& v0, const vec2& v1, std::vector<vec2>& res) {
    vec2 d = vec2::normalize(v1 - v0);
    float len = vec2::length(v1 - v0);
    float slen = len / res.size();


    for(size_t i=0; i < res.size(); i++) {
        float m = slen * i;
        auto& vec = res[i];
        vec.x = v0.x + d.x * m;
        vec.y = v0.y + d.y * m;
    }

    for(size_t i=0; i < res.size() - 1; i++) {
        auto& a = res[i];
        auto& b = res[i + 1];

        vec2 n = vec2::normal_of(b - a);
        a.x += n.x * randf(slen);
        a.y += n.y * randf(slen);
    }
    res.front() = v0;
    res.back()  = v1;
}

void sloped(float angle, float len, std::vector<vec2>& res) {
    
    float slen = len / res.size();

    for(size_t i=0; i < res.size(); i++) {
        vec2& vec = res[i];
        vec.x = SCREEN_TO_WORLD(i * slen);
        vec.y = SCREEN_TO_WORLD(std::sin(M_2_PI / slen * i)) * 10.0f;
    }
}

//helper
void fractal(float angle, std::vector<vec2>& res) {
    return fractal({0.0f, 0.0f}, vec2::from_angle(angle), res);
}

struct terrain {

    struct slice {
        b2Fixture* fixture_;
        b2EdgeShape shape_;
    };

    struct chunk {
        b2Body* body_;
        std::vector<slice> slices_;

        static void random(chunk& ref, int) {
            //float sliceWidth = (float)w / ref.slices_.size();

            std::vector<vec2> r(ref.slices_.size());
#ifdef FRACTAL_TERRAIN
            vec2 start = { 0, 0 };
            vec2 end = { 10, 2 };
            fractal(start, end, r);
#else
            sloped(0.0f, 128.0f, r);
#endif

            //DEBUG_VALUE_OF(r);

            for(size_t i=0; i < ref.slices_.size() - 1; i++) {
                auto& s = ref.slices_[i];


                b2EdgeShape* edge = static_cast<b2EdgeShape*>(s.fixture_->GetShape());
                edge->Set({r[i    ].x, r[i    ].y},
                          {r[i + 1].x, r[i + 1].y});
            }
        }
        static void from_json(const json&, chunk&);
    };

    std::vector<chunk> chunks_;

    static void create(terrain& self, b2World* world, size_t n, size_t chunksize) {
        self.chunks_.resize(n);
        for(size_t i=0; i < n; i++) {
            chunk& res = self.chunks_[i];
            b2BodyDef info;
            info.type = b2_staticBody;
            info.position.Set(0.0f, 0.0f);
            res.body_ = world->CreateBody(&info); 
        }

        
        for(auto it=self.chunks_.begin(); it != self.chunks_.end(); ++it) {
            chunk& c = (*it);
            c.slices_.resize(chunksize);
            std::generate(c.slices_.begin(), c.slices_.end(), [c] (void) {
                slice s;
                b2FixtureDef info;
                info.shape = &s.shape_;
                s.fixture_ = c.body_->CreateFixture(&info);
                return s;
            });
        }
    }

    static void init_random(terrain& self) {
        for(auto& it : self.chunks_) {
            chunk::random(it, 128);
        }
    }
};

#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>

using namespace wee;

void b2DebugDraw(b2World* world, SDL_Renderer* renderer, const SDL_Rect& camera, float zoom) {

    int cx = camera.x + (camera.w >> 1);
    int cy = camera.y + (camera.h >> 1);

    for(b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
        for(b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            b2Shape::Type shapeType = fixture->GetType();

            if ( shapeType == b2Shape::e_circle ){
                b2CircleShape* circleShape = (b2CircleShape*)fixture->GetShape();
            } else if ( shapeType == b2Shape::e_polygon ) {
                b2PolygonShape* polygonShape = (b2PolygonShape*)fixture->GetShape();
            } else if(shapeType == b2Shape::e_edge) {
                b2EdgeShape* edgeShape = static_cast<b2EdgeShape*>(fixture->GetShape());
                const b2Vec2& a = WORLD_TO_SCREEN(edgeShape->m_vertex1);
                const b2Vec2& b = WORLD_TO_SCREEN(edgeShape->m_vertex2);

                SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::IndianRed);

                SDL_RenderDrawLine(renderer, 
                    cx + (int)(a.x + 0.5f), 
                    cy + (int)(a.y + 0.5f), 
                    cx + (int)(b.x + 0.5f), 
                    cy + (int)(b.y + 0.5f)
                );
            }
        }
    }
}

struct game : applet {
    SDL_Rect camera_;
    float zoom_;
    b2World* world_;
    terrain t_;

    virtual int load_content() { 
        world_ = new b2World({0.0f, 9.8f});
        world_->SetContactListener(new collisions);
        terrain::create(t_, world_, 10, 32);
        terrain::init_random(t_);

        camera_ = {
            0, 0, 0, 0
        };
        return 0; 
    }
    virtual int update(int) { 
        
        return 0; 
    }
    virtual int draw(SDL_Renderer* renderer) { 
        
        SDL_RenderGetLogicalSize(renderer, &camera_.w, &camera_.h);
        {
            SDL_SetRenderTarget(renderer, NULL);
            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);//IndianRed);
            SDL_RenderClear(renderer);


            b2DebugDraw(world_, renderer, camera_, zoom_);

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
