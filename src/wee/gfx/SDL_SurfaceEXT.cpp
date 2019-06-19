#include <SDL.h>

#include <gfx/SDL_SurfaceEXT.hpp>

void SDL_PlotPixelEXT(SDL_Surface* surface, int x, int y, uint32_t pixel) {
        uint32_t* pixels = (uint32_t*)surface->pixels;
        uint32_t* dst = &pixels[x + y * surface->w];
        *dst = pixel;
    }

SDL_Surface* SDL_CreateSurfaceWithEXT(int w, int h, int rmask, int gmask, int bmask, int amask, const std::function<uint32_t(int, int)>& fn) {
        SDL_Surface* res = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
        for(int y=0; y < h; y++) {
            for(int x=0; x < w; x++) {
                SDL_PlotPixelEXT(res, x, y, fn(x, y));
            }
        }
        return res;
    }
