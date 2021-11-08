#include <gfx/SDL_ColorEXT.hpp>
#include <SDL.h>

void SDL_CreateColorRGB(uint8_t r, uint8_t g, uint8_t b, SDL_Color* ptr) {
	ptr->r = r;
	ptr->g = g;
	ptr->b = b;
	ptr->a = 255;
}

void SDL_CreateColorHSV(uint8_t h, uint8_t s, uint8_t v, SDL_Color* ptr) {
    if(s == 0)
        return SDL_CreateColorRGB(v, v, v, ptr);
    uint8_t region  = h / 43;
    uint8_t fpart   = (h - (region * 43)) * 6;
    uint8_t p       = (v * (255 - s)) >> 8;
    uint8_t q       = (v * (255 - ((s * fpart) >> 8))) >> 8;
    uint8_t t       = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;
    
    switch(region) {
        case 0: return  SDL_CreateColorRGB(v, t, p, ptr);
        case 1: return  SDL_CreateColorRGB(q, v, p, ptr);
        case 2: return  SDL_CreateColorRGB(p, v, t, ptr);
        case 3: return  SDL_CreateColorRGB(p, q, v, ptr);
        case 4: return  SDL_CreateColorRGB(t, p, v, ptr);
        default: return SDL_CreateColorRGB(v, p, q, ptr);
    }
    return SDL_CreateColorRGB(0, 0, 0, ptr);
}

