
#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <engine/assets.hpp>
#include <engine/sprite_font.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <fstream>


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


        
        //SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::White);
        SDL_RenderCopy(renderer, _font->_texture, NULL, NULL);

        SDL_RenderPresent(renderer);

    }

};


int main(int, char* []) {
    wee::application app(new game);
    return app.start();
}
