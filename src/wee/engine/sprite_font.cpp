
#include <engine/sprite_font.hpp>

using namespace wee;
    void measure_string(wee::sprite_font* font, const std::string& what, int* w, int* h) {
        int x = 0;
        int y = 0;
		const wee::font_info& finfo = font->_info;
//		int yy = y - finfo.height - finfo.descent;// + (finfo.ascent + finfo.descent);
        for(const auto& c : what) {
            const wee::glyph_info& info = font->_ginfo[c];
            //const SDL_Rect& src = font->get(c);
            x += info.advance;
            y = std::max(y, finfo.ascent - finfo.descent);
        }
        *w = x;
        *h = y;
    }
	void draw_string(SDL_Renderer* renderer, 
            wee::sprite_font* font, 
            const std::string& what, 
            const SDL_Point& where, 
            const SDL_Color& color) {
            


        SDL_SetTextureColorMod(font->_texture, color.r, color.g, color.b);;

        int x = where.x;
        int y = where.y;
        //  get baseline
        const wee::font_info& finfo = font->_info;
        int yy = y - finfo.height - finfo.descent;// + (finfo.ascent + finfo.descent);
        for(const auto& c : what) {
            const wee::glyph_info& info = font->_ginfo[c];
            const SDL_Rect& src = font->get(c);
            SDL_Rect dst = {
                x + info.minx, 
                yy,// - info.maxy,
                src.w, src.h
            };
            SDL_RenderCopy(renderer, font->_texture, &src, &dst);
            x += info.advance;
        }
	}

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
