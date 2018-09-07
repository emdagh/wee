#undef KULT_BUILD_TESTS
#include <kult.hpp>
#include <nlohmann/json.hpp>
#include <Box2D/Box2D.h>
#include <SDL.h>

#ifndef PTM_RATIO
#define PTM_RATIO   (40.f)
#endif

#define WORLD_TO_SCREEN(x)  ((x) * (PTM_RATIO))
#define SCREEN_TO_WORLD(x)  ((x) / (PTM_RATIO))

using nlohmann::json;
using kult::entity;

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
    virtual void BeginContact(b2Contact* contact) {
    }
    virtual void EndContact(b2Contact* contact) {
    }
};


entity create_player(b2World* world, float x, float y) {
    entity res;
    
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(SCREEN_TO_WORLD(x), SCREEN_TO_WORLD(y));
    
    b2CircleShape shape;
    shape.m_p = { 0.0f, 0.0f };
    shape.m_radius = SCREEN_TO_WORLD(10.0f);

    b2FixtureDef fd;
    fd.userData = static_cast<void*>(&res);
    fd.filter.maskBits = 0;
    fd.filter.categoryBits = 0;
    fd.shape = &shape;


    res[rigidbody] = (rigidbody_t){ world->CreateBody(&bd) };
    res[collider] = (collider_t) { 
        res[rigidbody].body_->CreateFixture(&fd),
            NULL,
            NULL

    };

    return res;

    //world->physics[id].body = world->sim->CreateBody(&bdef);
}

int main(int, char* []) {
    collisions collisions_;
    auto* simulation_ = new b2World({0.0f, 9.8f});
    simulation_->SetContactListener(&collisions_);
    entity hero = create_player(simulation_, 0.0f, 0.0f);

    static float fDt = 16.667;
    while(1) {
        simulation_->Step(fDt, 8, 3, 4);
        std::cout << hero[rigidbody].body_->GetTransform() << std::endl;

    }
    return 1;
}
