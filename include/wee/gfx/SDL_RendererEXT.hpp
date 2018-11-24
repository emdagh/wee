#ifndef SDL_RENDERER_EXT_H_
#define SDL_RENDERER_EXT_H_

#include <wee/wee.h>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;
struct SDL_Color;


C_API void SDL_RenderCopyEXT(struct SDL_Renderer* renderer, struct SDL_Texture* texture, const SDL_Rect& src, const SDL_Rect& dst);
C_API void SDL_SetRenderDrawColorEXT(struct SDL_Renderer* renderer, const SDL_Color& color);

C_API void SDL_RenderDrawCircleEXT(SDL_Renderer* renderer, int x, int y, int r); 
C_API void SDL_RenderDrawCircleFilledEXT(SDL_Renderer* renderer, int x, int y, int r); 


#endif
