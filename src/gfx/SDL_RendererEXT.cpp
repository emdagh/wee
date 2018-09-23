#include <gfx/SDL_RendererEXT.hpp>
#include <SDL.h>
#include <math.h>

void SDL_RenderCopyEXT(struct SDL_Renderer* renderer, struct SDL_Texture* texture, const SDL_Rect& src, const SDL_Rect& dst)
{
    SDL_RenderCopy(renderer, texture, &src, &dst);
}
void SDL_SetRenderDrawColorEXT(struct SDL_Renderer* renderer, const SDL_Color& color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void SDL_RenderDrawCircleEXT(SDL_Renderer* renderer, int x, int y, int r) {
    static const int n = 15;

    float rad = (M_PI * 2.0f) / (float)n;

    for(int i=0; i < n ; i++) {
        float x0 = x + (float)cos((double)rad * i) * (float)r;
        float y0 = y + (float)sin((double)rad * i) * (float)r;

        float x1 = x + (float)cos((double)rad * (i + 1)) * (float)r;
        float y1 = y + (float)sin((double)rad * (i + 1)) * (float)r;

        SDL_RenderDrawLine(renderer, 
            (int)(x0 + 0.5f), 
            (int)(y0 + 0.5f), 
            (int)(x1 + 0.5f), 
            (int)(y1 + 0.5f)
        );
    }
}
