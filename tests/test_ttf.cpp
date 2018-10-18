#include <wee/wee.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <engine/assets.hpp>
#include <engine/sprite_font.hpp>
#include <core/logstream.hpp>
#include <nlohmann/json.hpp>
//std::map<uint16_t, SDL_Rect> index;
//
//
using namespace wee;
namespace json_helper {

using nlohmann::json;

template <typename T>
void iterate(const json& j, T f) {
    for(auto it=j.begin(); it != j.end(); it++) {
        if(it->is_structured()) {
            iterate(*it, f);
        } else {
            f(it);
        }
    }

}
}


#include <base/application.hpp>
#include <base/applet.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
struct game : wee::applet {
    sprite_font* _sf;

    game() {
    }

    virtual ~game() {

        TTF_Quit();
    }

#define USE_ASSETS

    int load_content() {
        assets<TTF_Font>::instance();


#ifdef USE_ASSETS
        auto load_ttf = [&] (std::istream& is, int size) {
            std::istreambuf_iterator<char> eos;
            std::string contents(std::istreambuf_iterator<char>(is),
                (std::istreambuf_iterator<char>())
            );
            return TTF_OpenFontRW(SDL_RWFromConstMem(contents.c_str(), (int)contents.length()), 1, size);
        };
        std::string pt = wee::get_resource_path("assets") + "ttf/BlackCastleMF.ttf";//Boxy-Bold.ttf";
        std::ifstream is;
        is.open(pt);
        if(!is.is_open()) {
            throw ::file_not_found(pt);
        }
        auto* f = load_ttf(is, 32);
        _sf = new sprite_font("@foofont", f);
#else

        std::string pt = wee::get_resource_path("assets") + "ttf/BlackCastleMF.ttf";//Boxy-Bold.ttf";
        std::ifstream is;
        is.open(pt);
        if(!is.is_open()) {
            throw ::file_not_found(pt);
        }
        std::istreambuf_iterator<char> eos;
        std::string contents(std::istreambuf_iterator<char>(is),
            (std::istreambuf_iterator<char>())
        );
        TTF_Font* font = TTF_OpenFontRW(SDL_RWFromConstMem(contents.c_str(), (int)contents.length()), 1, 32);
        _sf = new sprite_font("@foofont", font);

#endif



        return 0;
    }
    int update(int dt) {
        return 0;
    }
    int draw(SDL_Renderer* renderer) {
        //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);

        /*SDL_Rect r = { 0, 0, 0, 0 };
        SDL_QueryTexture(_sf->_texture, NULL, NULL, &r.w, &r.h);
        SDL_RenderCopy(renderer, _sf->_texture, &r, &r);*/

        const std::string test = "The quick brown fox jumps over the lazy dog.";
        const font_info& finfo = _sf->_info;
        int x = 0;
        int y = 240;
        


        int yy = y - finfo.height - finfo.descent;// + (finfo.ascent + finfo.descent);
        

        //SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::White);
        for(auto c : test) {
            const glyph_info& info = _sf->_ginfo[c];
            const SDL_Rect& src = _sf->get(c);
            SDL_Rect dst = {
                x + info.minx, 
                yy,// - info.maxy,
                src.w, src.h
            };
            SDL_RenderCopy(renderer, _sf->_texture, &src, &dst);
            x += info.advance;
        }

        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::LightGrey);
        SDL_RenderDrawLine(renderer, 0, y, 640, y);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Red);
        SDL_RenderDrawLine(renderer, 0, y - finfo.height, 640, y - finfo.height);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Lime);
        SDL_RenderDrawLine(renderer, 0, y - finfo.ascent, 640, y - finfo.ascent);
        SDL_RenderDrawLine(renderer, 0, y - finfo.descent, 640, y - finfo.descent);


        SDL_RenderPresent(renderer);
        return 0;
    }
};

int main(int, char* []) {
	
	try {
        wee::application app(new game);

        return app.start();
    } catch(const std::exception& e) {
        DEBUG_ERROR(e.what());
    }

    return 0;
}
