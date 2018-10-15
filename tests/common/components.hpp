#pragma once

#include <functional>
#include <SDL.h>
#include <kult.hpp>
#include <nlohmann/json.hpp>

#include <core/vec2.hpp>
#include <Box2D/Box2D.h>

struct collision {
    kult::type self;
    kult::type other;
    wee::vec2 point;
    wee::vec2 normal;
};
struct raycast_hit {
    kult::type self;
    /*wee::vec2 point;
    wee::vec2 normal;
    float fraction;*/
} ;

#define DEFAULT_COLLIDER_CALLBACK [] (const collision&) {} 

typedef std::function<void(const collision&)> collision_callback;
typedef std::function<void(const raycast_hit&)> raycast_callback;

typedef struct {
    b2Fixture* fixture = NULL;
    collision_callback enter = DEFAULT_COLLIDER_CALLBACK;
    collision_callback leave = DEFAULT_COLLIDER_CALLBACK;
} collider_t; 

typedef struct {
    b2Body* body;
} rigidbody_t;

#define DEFAULT_RAYCAST_CALLBACK [] (const raycast_hit& ) { }

typedef struct {
    raycast_callback enter = DEFAULT_RAYCAST_CALLBACK;
    bool is_hit;
    wee::vec2 point;
    wee::vec2 normal;
    float fraction;
} raycast_t;

typedef struct {
    wee::vec2 p;
    float t;
} transform_t;

typedef struct {
    kult::type parent;
    wee::vec2 offset;
} nested_t;

typedef struct {
    b2Joint* joint;
} articulation_t;


typedef struct {
    SDL_Texture* texture = NULL;
    SDL_Rect     src;
    SDL_Color    color = {255,255,255,255};
    int layer;
} visual_t;


typedef struct {
    float* dst;
    std::function<float(float)> easing;
} tween_t;

typedef struct {
    int time;
    int timeout;
    std::function<void(const kult::type&)> on_timeout = nullptr;
} timeout_t;

typedef struct {
    wee::vec2 last;
    kult::type next;
} terrain_t;




std::ostream& operator << (std::ostream& os, const rigidbody_t&) ;
std::ostream& operator << (std::ostream& os, const transform_t&);
std::ostream& operator << (std::ostream& os, const nested_t& );
std::ostream& operator << (std::ostream& os, const visual_t& );
std::ostream& operator << (std::ostream& os, const terrain_t& t);
std::ostream& operator << (std::ostream& os, const collider_t& t);
std::ostream& operator << (std::ostream& os, const raycast_t&);
std::ostream& operator << (std::ostream& os, const timeout_t&);
std::ostream& operator << (std::ostream& os, const articulation_t&);

using collider  = kult::component<1 << 0, collider_t>;
using rigidbody = kult::component<1 << 1, rigidbody_t>;
using nested    = kult::component<1 << 2, nested_t>;
using transform = kult::component<1 << 3, transform_t>;
using visual    = kult::component<1 << 4, visual_t>;
using terrain   = kult::component<1 << 5, terrain_t>;
using raycast   = kult::component<1 << 6, raycast_t>;
using articulation = kult::component<1 << 8, articulation_t>;
using timeout     = kult::component<1 << 9, timeout_t>;
