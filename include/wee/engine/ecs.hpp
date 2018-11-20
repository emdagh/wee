#pragma once

#include <kult/kult.hpp>
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


    typedef kult::type entity_t;
    struct collision {
        entity_t self;
        entity_t other;
        vec2f n;
        vec2f point;
    };
    typedef std::function<void(const collision&)> collision_callback;

#define DEFAULT_COLLISION_CALLBACK [] (const collision&) { }

    typedef struct {
        b2Body* body    = nullptr;

        collision_callback on_collision_enter = DEFAULT_COLLISION_CALLBACK;
        collision_callback on_collision_leave = DEFAULT_COLLISION_CALLBACK;
        collision_callback on_trigger_enter   = DEFAULT_COLLISION_CALLBACK;
        collision_callback on_trigger_leave   = DEFAULT_COLLISION_CALLBACK;
    } physics_t;

    typedef  struct {
        vec2f position = { 0.f, 0.f };
        float rotation = 0.f;
    } transform_t;

    typedef struct {
        SDL_Texture*        texture     = nullptr;
        SDL_Rect            src;
        SDL_Color           color;
        SDL_RendererFlip    flip        = SDL_FLIP_NONE;
        bool                visible     = true;
    } visual_t;

    typedef struct {
        vec2f    offset = { 0.f, 0.f};
        float    rotation = 0.0f;
        entity_t parent = kult::none();
    } nested_t;

    
    typedef struct {
        float* dst;
        std::function<float(float)> easing_fn;
    } tween_t;

    typedef struct {
        int time;
        int timeout;
        std::function<void(const entity_t&)> on_timeout = nullptr;
    } timeout_t;

    typedef struct {
        bool  hit       = false;
        vec2f point     = { 0.f, 0.f };
        vec2f n         = { 0.f, 0.f };// normal
        float d         = -1.0f; // length of fraction
    } raycast_t;


    typedef struct {
        b2Joint* joint;
        //bool do_cleanup;
    } joint_t;
    
    std::ostream& operator << (std::ostream&, const physics_t&);
    std::ostream& operator << (std::ostream&, const transform_t&);
    std::ostream& operator << (std::ostream&, const visual_t&);
    std::ostream& operator << (std::ostream&, const nested_t&);
    std::ostream& operator << (std::ostream&, const tween_t&);
    std::ostream& operator << (std::ostream&, const timeout_t&);
    std::ostream& operator << (std::ostream&, const raycast_t&);
    std::ostream& operator << (std::ostream&, const joint_t&);
    

    using physics   = kult::component<1,  physics_t>;
    using transform = kult::component<2,transform_t>;
    using visual    = kult::component<3,   visual_t>;
    using nested    = kult::component<4,   nested_t>;
    using tween     = kult::component<5,    tween_t>;
    using timeout   = kult::component<6,  timeout_t>;
    using raycast   = kult::component<7,  raycast_t>;
    using joint     = kult::component<8,   joint_t>;



}
