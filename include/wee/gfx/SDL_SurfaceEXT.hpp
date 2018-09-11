#pragma once

#include <wee/wee.h>
#include <functional>

struct SDL_Surface;

C_API void SDL_PlotPixelEXT(SDL_Surface* surface, int x, int y, uint32_t pixel);

C_API SDL_Surface* SDL_CreateSurfaceWithEXT(int w, 
        int h, 
        int rmask, 
        int gmask, 
        int bmask, 
        int amask, 
        const std::function<uint32_t(int, int)>& fn); 
/*
        SDL_Surface* surf = SDL_CreateSurfaceWithEXT(n, n, 0, 0, 0, 0, [&] (int x, int y) {
            float frequency = 16.0f;
            float fx = n / frequency;
            float fy = n / frequency;
            uint32_t r = (uint32_t)(255 * simplex_noise(x / fx, y / fy, 0.0f));

            uint32_t m = (0xff << 24) | (r << 16) | (r << 8) | (r);

            return m;
        });
        assets<SDL_Texture>::instance().from_surface("@simplex_noise", surf);
*/
