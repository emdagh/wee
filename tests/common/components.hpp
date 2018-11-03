#pragma once
#include <engine/ecs.hpp>
#include <functional>
#include <SDL.h>
#include <kult.hpp>
#include <nlohmann/json.hpp>

#include <core/vec2.hpp>
#include <core/logstream.hpp>
#include <Box2D/Box2D.h>


typedef struct {
    wee::vec2       last;
    wee::entity_t   next;
} terrain_t;
std::ostream& operator << (std::ostream& os, const terrain_t& t);

using terrain   = kult::component<1 << 5, terrain_t>;

typedef struct {
    int score = 0;
    int hp = 100;
} player_t;
std::ostream& operator << (std::ostream& os, const player_t&);
using player   = kult::component<1 << 7, player_t>;

