#include <gfx/SDL_RendererEXT.hpp>
#include <SDL.h>

void SDL_RenderCopyEXT(struct SDL_Renderer* renderer, struct SDL_Texture* texture, const SDL_Rect& src, const SDL_Rect& dst)
{
    SDL_RenderCopy(renderer, texture, &src, &dst);
}
void SDL_SetRenderDrawColorEXT(struct SDL_Renderer* renderer, const SDL_Color* color)
{
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}

