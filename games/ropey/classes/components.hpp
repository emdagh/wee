#pragma once

#include <engine/ecs.hpp>

typedef struct {
    int score;
} player_t;

using player = kult::component<1 << 11, player_t>;

typedef struct {
    int value;
    int type;
} pickup_t;
using pickup = kult::component<1 << 10, pickup_t>;
/**
 * NOTE: is a `beat` a component? it sure feels more like an entity
 */
struct beat_t {
    wee::vec2f spawn; 
    wee::vec2f offset;
    int difficulty;
    int respawn = 0;
    float width;
};
using beat = kult::component<1 << 12, beat_t>;

struct synch_t {
    bool cleanup = false;
};
using synch = kult::component<1 << 13, synch_t>;


std::ostream& operator << (std::ostream& os, const player_t& p) ;
std::ostream& operator << (std::ostream& os, const pickup_t& p) ;
std::ostream& operator << (std::ostream& os, const beat_t& p) ;
std::ostream& operator << (std::ostream& os, const synch_t& p) ;
