
#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/assets.hpp>
#include <engine/sprite_font.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <fstream>

namespace {
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
}

struct game : wee::applet {
    wee::sprite_font* _font;
    int load_content() {
        std::string pt = wee::get_resource_path("assets") + "ttf/BlackCastleMF.ttf";//Boxy-Bold.ttf";
		//std::ifstream is(pt);
        _font = new wee::sprite_font("@foofont",
	        wee::assets<TTF_Font>::instance().load("@foofont", 32, ::as_lvalue(std::ifstream(pt)))
		);

        return 0;
    }
    int update(int) {}
    int draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);


        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::IndianRed);
        ::draw_string(renderer, _font, "The quick brown fox", { 10, 128 }, SDL_ColorPresetEXT::IndianRed);
        //SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::White);
        //SDL_RenderCopy(renderer, _font->_texture, NULL, NULL);

        SDL_RenderPresent(renderer);

    }

};


int main(int, char* []) {
    wee::application app(new game);
    return app.start();
}
