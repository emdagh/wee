#include <classes/entities.hpp>
#include <classes/components.hpp>
#include <core/logstream.hpp>
#include <engine/assets.hpp>
#include <engine/ecs.hpp>
#include <engine/camera.hpp>
#include <gfx/SDL_ColorEXT.hpp>
using namespace wee;

void destroy_entity(const entity_type& self) {
    /**
     * check if any nested entities are present,
     * if so: they should also be destroyed.
     */
    for(const auto& i : kult::join<nested>() ) {
        if(kult::get<nested>(i).parent == self) {
            destroy_entity(i);
        }
    }
    /**
     * physics need to be cleaned properly
     */
    if(kult::has<physics>(self)) {
        b2Body* rb = kult::get<physics>(self).body;

        for(auto* ptr = rb->GetFixtureList(); ptr; ptr = ptr->GetNext()) {
            rb->DestroyFixture(ptr);
        }
        b2World* world = rb->GetWorld();
        world->DestroyBody(rb);
    }
    /**
     * and finally, purge the entity.
     */
    kult::purge(self);
}
entity_type create_player(b2World* world, const vec2f& at) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
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
        fd.filter.categoryBits = E_CATEGORY_PLAYER;//!0;//(uint16_t)collision_filter::player;
        fd.filter.maskBits = E_CATEGORY_ENVIRONMENT | E_CATEGORY_PICKUP;//!0;//(uint16_t)collision_filter::any;
        fd.density = 1.0f;
        fd.restitution = 0.0f;
        fd.shape = &shape;
        fd.isSensor = false;
        fd.userData = reinterpret_cast<void*>(self);

        body->CreateFixture(&fd);
    }

    SDL_Texture* texture = nullptr;
    {
        auto is = open_ifstream("assets/img/player.png", std::ios::binary);
        assert(is.is_open());
        texture = assets<SDL_Texture>::instance().load("@player",is);

        kult::add<visual>(self);
        auto& v = kult::get<visual>(self);
        v.texture = texture;
        SDL_QueryTexture(texture, NULL, NULL, &v.src.w, &v.src.h);
        v.src.x = v.src.y = 0;
        v.visible = true;
        /*n.offset.x = -v.src.w / 2;
        n.offset.y = -v.src.h / 2;*/
    }
    kult::add<transform>(self);
    kult::add<physics>(self).body = body;
    {
        auto& pp = kult::add<player>(self);
        pp.score = 0;
        pp.hp = 3;
        pp.is_shake = false;
        pp.is_flash = false;
    }

    kult::get<physics>(self).on_collision_enter = [&] (const collision& col) {
        DEBUG_METHOD();
        DEBUG_VALUE_OF(col.self);
    };
    return self;
}

entity_type create_block(b2World* world, const SDL_Rect& r) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;

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
        
        body->CreateFixture(&def);
    }

    kult::add<physics>(self).body = body;
    kult::add<raycast>(self).hit = false;

    return self;
}

entity_type create_tile(SDL_Texture* texture, const SDL_Point& dst, const SDL_Rect& src, const SDL_RendererFlip& flip, float radians) {
    entity_type self = kult::entity();
    kult::add<visual>(self) = { 
        texture, src, SDL_ColorPresetEXT::White, flip, true
    };
    kult::add<transform>(self) = { vec2f{0.f, 0.f}, 0.f };//{ {(float)dst.x, (float)dst.y}, radians};
    kult::add<nested>(self).offset = {
        (float)(dst.x + (src.w >> 1)), 
        (float)(dst.y + (src.h >> 1))
    };
    kult::get<nested>(self).rotation = radians;
    return self;
}

entity_type create_rope(b2World* world, entity_type a, entity_type b, const b2Vec2& bPosWS) {


    b2Body* body = kult::get<physics>(b).body;
    if(!body) {
        throw std::logic_error("entity must have a physics attached");
    }
    entity_type e = kult::entity();

    b2RopeJointDef def;
    def.userData = reinterpret_cast<void*>(e);
    def.bodyA = kult::get<physics>(a).body; 
    def.bodyB = kult::get<physics>(b).body; 
    def.collideConnected = true;
    // center of player
    def.localAnchorA = b2Vec2 {0.0f, 0.0f};
    // raycast the world here
    def.localAnchorB = body->GetLocalPoint(bPosWS);
    def.maxLength = (def.bodyA->GetPosition() - bPosWS).Length();//SCREEN_TO_WORLD(0.0f);

    kult::add<joint>(e).joint = world->CreateJoint(&def);
    return e;
}
entity_type create_sensor(b2World* world, entity_type parent, const vec2f& offset, float radius, const collision_callback&) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
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
        body->CreateFixture(&def);

    }
    kult::add<physics>(self).body = body;
    kult::add<nested>(parent);
    {
        nested_t& n = kult::get<nested>(parent);
        n.parent = parent;
        n.offset = offset;
        
    }
    return self;
}




entity_type screen_flash(int, SDL_Renderer*) {
    /**
     * timer + visual
     * visual.texture -> whitePixel
     * timer.on_tick -> set alpha 
     */
    entity_type self = kult::entity();
    return self;
}

entity_type create_timer(int to, float* ptr) {
    entity_type self = kult::entity();
    timeout_t& t = kult::add<timeout>(self);
    t.time = 0;
    t.timeout = to;

    tween_t& w = kult::add<tween>(self);
    w.dst = ptr;
    w.easing_fn = &easing::ease_linear_in_out;
    
    //std::function<void(const entity_t&)> on_timeout = nullptr;
    return self;
}
