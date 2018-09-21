
#include <engine/sprite_font.hpp>

using namespace wee;

int TTF_GlyphSize(TTF_Font* font, Uint16 c, int* w, int* h) {
    if(!TTF_GlyphIsProvided(font, c)) {
        return -1;
    }

    int minx, miny;
    int maxx, maxy;
    
    TTF_GlyphMetrics(font, c, &minx, &maxx, &miny, &maxy, NULL);
    if(w != NULL) 
        *w = maxx - minx;

    if(h != NULL) 
        *h = maxy - miny;

    return 0;
}
