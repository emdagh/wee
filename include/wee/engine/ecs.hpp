#pragma once
#ifdef _MSC_VER
#undef interface
#endif
#include <functional>
#include <engine/ecs/ecs.hpp>
#include <Box2D/Box2D.h>
#include <engine/easing.hpp>
#include <core/vec2.hpp>
#include <SDL.h>
#include <iostream>

#ifndef PTM_RATIO
#define PTM_RATIO   (40.f)
#endif

#define WORLD_TO_SCREEN(x)  ((x) * (PTM_RATIO))
#define SCREEN_TO_WORLD(x)  ((x) / (PTM_RATIO))

namespace wee {


    //typedef kult::type entity_t;
    
    typedef ecs::id_type entity_t;

    struct collision {
        entity_t self;
        entity_t other;
        vec2f n;
        vec2f point;
    };
    typedef std::function<void(const collision&)> collision_callback;

#define DEFAULT_COLLISION_CALLBACK [] (const collision&) { }

    struct physics_t{
        b2Body* body    = nullptr;

        collision_callback on_collision_enter = DEFAULT_COLLISION_CALLBACK;
        collision_callback on_collision_leave = DEFAULT_COLLISION_CALLBACK;
        collision_callback on_trigger_enter   = DEFAULT_COLLISION_CALLBACK;
        collision_callback on_trigger_leave   = DEFAULT_COLLISION_CALLBACK;
    } ;

    struct transform_t{
        vec2f position = { 0.f, 0.f };
        float rotation = 0.f;
    } ;

    struct visual_t{
        SDL_Texture*        texture     = nullptr;
        SDL_Rect            src;
        SDL_Color           color;
        SDL_RendererFlip    flip        = SDL_FLIP_NONE;
        bool                visible     = true;
    } ;

    struct nested_t {
        vec2f    offset = { 0.f, 0.f};
        float    rotation = 0.0f;
        entity_t parent = ecs::none<entity_t>();
    } ;

    
    struct tween_t{
        float* dst;
        std::function<float(float)> easing_fn;
    } ;

    struct timeout_t {
        int time;
        int timeout;
        std::function<void(const entity_t&)> on_tick = nullptr;
        std::function<void(const entity_t&)> on_timeout = nullptr;
    } ;

    struct raycast_t {
        bool  hit       = false;
        vec2f point     = { 0.f, 0.f };
        vec2f n         = { 0.f, 0.f };// normal
        float d         = -1.0f; // length of fraction
    } ;


    struct joint_t {
        b2Joint* joint;
        //bool do_cleanup;
    };
    
    std::ostream& operator << (std::ostream&, const physics_t&);
    std::ostream& operator << (std::ostream&, const transform_t&);
    std::ostream& operator << (std::ostream&, const visual_t&);
    std::ostream& operator << (std::ostream&, const nested_t&);
    std::ostream& operator << (std::ostream&, const tween_t&);
    std::ostream& operator << (std::ostream&, const timeout_t&);
    std::ostream& operator << (std::ostream&, const raycast_t&);
    std::ostream& operator << (std::ostream&, const joint_t&);
    

    using physics   = ecs::component<1,  physics_t>;
    using transform = ecs::component<2,transform_t>;
    using visual    = ecs::component<3,   visual_t>;
    using nested    = ecs::component<4,   nested_t>;
    using tween     = ecs::component<5,    tween_t>;
    using timeout   = ecs::component<6,  timeout_t>;
    using raycast   = ecs::component<7,  raycast_t>;
    using joint     = ecs::component<8,   joint_t>;



}
