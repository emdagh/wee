#pragma once

#include <engine/ecs.hpp>

namespace wee {

auto copy_transform_to_physics = [] () {
    for(auto& e : kult::join<transform, physics>()) {
        const vec2& p = kult::get<transform>(e).position;
        float r = kult::get<transform>(e).rotation;
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
        kult::get<transform>(e).rotation   = kult::get<physics>(e).body->GetAngle();
    }
};

auto clean_physics = [] (b2World* world) {
    for(auto& self : kult::join<physics>()) {
        if(!kult::get<physics>(self).do_cleanup) 
            continue;
        b2Body* rb = kult::get<physics>(self).body;

        for(auto* ptr = rb->GetFixtureList(); ptr; ptr = ptr->GetNext()) {
            rb->DestroyFixture(ptr);
        }
        world->DestroyBody(rb);
        kult::purge(self);
    }
};
}
