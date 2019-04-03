#pragma once


#include <gfx/texture_address_mode.hpp>
#include <gfx/texture_filter.hpp>

struct SDL_Texture;

namespace wee {
    struct texture_sampler {
        SDL_Texture* texture = nullptr;
        texture_address_mode address_u = texture_address_mode::kClamp;
        texture_address_mode address_v = texture_address_mode::kClamp;
        texture_address_mode address_w = texture_address_mode::kClamp;
        texture_filter filter = texture_filter::kLinear;
        int unit = 0;
    };
}
