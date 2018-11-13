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

std::ostream& operator << (std::ostream& os, const player_t& p) ;
std::ostream& operator << (std::ostream& os, const pickup_t& p) ;
