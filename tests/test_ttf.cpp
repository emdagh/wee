#include <wee/wee.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <engine/assets.hpp>
#include <engine/sprite_font.hpp>
#include <util/logstream.hpp>
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

void test_spritefont(const std::string& pt) {
    using nlohmann::json;
    std::ifstream is;
    is.open(pt);
    if(!is.is_open()) {
        throw ::file_not_found(pt);
    }

    json::parser_callback_t cb = [](int depth, json::parse_event_t event, json & parsed) {

        return true;
    };

    json::parse(is, cb);
    is.close();
}

#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
struct game : wee::applet {
    sprite_font* _sf;

    game() {
    }

    virtual ~game() {

        TTF_Quit();
    }

#undef USE_ASSETS

        TTF_Font* load_ttf(const std::string& name, int size, std::istream& is) {
            std::istreambuf_iterator<char> eos;
            std::string contents(std::istreambuf_iterator<char>(is),
                (std::istreambuf_iterator<char>())
            );

            SDL_RWops* rw = SDL_RWFromConstMem(contents.c_str(), (int)contents.length());

            TTF_Font* font = TTF_OpenFontRW(rw, 0, 32);


            return font;
        }
    int load_content() {


        assets<TTF_Font>::instance();


        std::string pt = wee::get_resource_path("assets") + "ttf/BlackCastleMF.ttf";//Boxy-Bold.ttf";
        std::ifstream is;
        is.open(pt);
        if(!is.is_open()) {
            throw ::file_not_found(pt);
        }

#ifdef USE_ASSETS
        TTF_Font* font = load_ttf("@foo", 32, is);
#else

        std::istreambuf_iterator<char> eos;
        std::string contents(std::istreambuf_iterator<char>(is),
            (std::istreambuf_iterator<char>())
        );
        //SDL_RWops* rw = SDL_RWFromConstMem(contents.c_str(), (int)contents.length());
        TTF_Font* font = TTF_OpenFontRW(SDL_RWFromConstMem(contents.c_str(), (int)contents.length()), 0, 32);

#endif

        _sf = new sprite_font("@foofont", font);


        return 0;
    }
    int update(int dt) {
        return 0;
    }
    int draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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
        LOGE(e.what());
    }

    return 0;
}
