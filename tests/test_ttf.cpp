#include <wee/wee.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <engine/assets.hpp>
#include <util/logstream.hpp>
#include <nlohmann/json.hpp>
//std::map<uint16_t, SDL_Rect> index;
//

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define SDL_RMASK   0xff000000
#define SDL_GMASK   0x00ff0000
#define SDL_BMASK   0x0000ff00
#define SDL_AMASK   0x000000ff
#else
#define SDL_AMASK   0xff000000
#define SDL_BMASK   0x00ff0000
#define SDL_GMASK   0x0000ff00
#define SDL_RMASK   0x000000ff
#endif
std::ostream& operator << (std::ostream& os, const SDL_Rect& rc) {
    using nlohmann::json;
    json j = {
        "x", rc.x,
        "y", rc.y,
        "w", rc.w,
        "h", rc.h
    };
    return os << j;
}

float SDL_GetRectArea(const SDL_Rect& r) {
    return (float)r.w * r.h;
}

int operator < (const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_GetRectArea(a) < SDL_GetRectArea(b);
}

int operator == (const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_GetRectArea(a) == SDL_GetRectArea(b);
}


namespace packer {
    constexpr static const int INVALID = -1;
    struct node {
        node* left = nullptr, *right = nullptr;
        int id = -1;
        SDL_Rect rc;
    };

    void iterate(node* n, std::function<void(int, const SDL_Rect&)> fn) {
        fn(n->id, n->rc);

        if(n->left) 
            iterate(n->left, fn);
        if(n->right)
            iterate(n->right, fn);            
    }

    node* insert(node* n, const SDL_Rect& rc) {
        if(n->left || n->right) {
            node* nn = insert(n->left, rc);
            return nn ? nn : insert(n->right, rc);
        } else {
            if(n->id != INVALID || (n->rc < rc)) {
                return NULL;
            }

            if(rc == n->rc) {
                return n;
            }

            n->left  = new node;
            n->right = new node;

            int dw = n->rc.w - rc.w;
            int dh = n->rc.h - rc.h;
            if(dw > dh) {
                n->left->rc  = { 
                    n->rc.x, 
                    n->rc.y, 
                    rc.w, 
                    n->rc.h 
                };
                n->right->rc = { 
                    n->rc.x + rc.w,
                    n->rc.y, 
                    n->rc.w - rc.w,
                    n->rc.h
                };
            } else {
                n->left->rc = {
                    n->rc.x,
                    n->rc.y,
                    n->rc.w,
                    rc.h
                };

                n->right->rc = {
                    n->rc.x,
                    n->rc.y + rc.h,
                    n->rc.w,
                    n->rc.h - rc.h
                };

            }
            return insert(n->left, rc);
        }
    }
}



template <typename T = int>
struct sprite_sheet {
    std::map<T, size_t> _ix;
    std::vector<SDL_Rect> _rects;
    SDL_Texture* _texture;

    const SDL_Rect* get(const T& key) {
        if(_ix.count(key) > 0) {
            return &_rects[_ix[key]];
        }
        return nullptr;
    }

    void add(const T& key, const SDL_Rect& value) {
        if(_ix.count(key) == 0) {
            _ix[key] = _rects.size();
            _rects.push_back(value);
        }
    }
};

namespace math {

    constexpr int log2(int n) {
        return ( (n<2) ? 0 : 1 + log2(n>>1));
    }

    template <typename T>
    T npot(const T& n) {
        return 1 << (log2(n - 1) + 1);
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

struct font_info {
    int height;
    int ascent;
    int descent;
    int lineskip;
};

struct glyph_info {
    int minx    = 0;
    int maxx    = 0;
    int miny    = 0;
    int maxy    = 0;
    int advance = 0;
};

template <typename T = char>
struct basic_sprite_font : sprite_sheet<T> {

    TTF_Font* _font;
    std::string _name;
    font_info _info;
    std::map<T, glyph_info> _ginfo;

    void _texture_size(int n, int maxw, int maxh, int* w) {
        float a = n * maxw * maxh; 
        float x = std::sqrt(a);
        float d = std::round(x); 
        *w = math::npot(((int)d));
    }

    basic_sprite_font(const std::string& name, TTF_Font* font) 
        : _font(font) 
        , _name(name)
    {
        _info.height    = TTF_FontHeight(font);
        _info.ascent    = TTF_FontAscent(font);
        _info.descent   = TTF_FontDescent(font);
        _info.lineskip  = TTF_FontLineSkip(font);
        build();
    }


    void build() {
        int maxw = 0, maxh = 0;
        int w, h;
        int num = 0;
        for(unsigned char a=32; a < 255; a++) {
            if(TTF_GlyphIsProvided(_font, a)) {

                TTF_GlyphSize(_font, (uint16_t)a, &w, &h);
                maxw = std::max(w, maxw);
                maxh = std::max(h, maxh);
                num++;
            }
        }

        int d = 0;
        _texture_size(num, maxw, maxh, &d);

        SDL_Surface* surface = SDL_CreateRGBSurface(0, d, d, 32,
                SDL_RMASK, SDL_GMASK, SDL_BMASK, SDL_AMASK);

        
        packer::node* root = new packer::node;
        root->id = -1;
        root->left = root->right = NULL;
        root->rc = { 0, 0, d, d };
        for(unsigned char a=32; a < 128; a++) {
            //TTF_GlyphSize(_font, (uint16_t)a, &n->rc.w, &n->rc.h, NULL);
            if(!TTF_GlyphIsProvided(_font, a)) 
                continue;

            glyph_info gi;
            TTF_GlyphMetrics(_font, (uint16_t)a,
                    &gi.minx,
                    &gi.maxx,
                    &gi.miny,
                    &gi.maxy,
                    &gi.advance);
            _ginfo[a] = gi;

            SDL_Surface* tmp = TTF_RenderGlyph_Blended(_font, a, {0xff, 0xff, 0xff, 0x00});
            packer::node* n = packer::insert(root, {0, 0, tmp->w, tmp->h});
            n->id = (int)a;

            sprite_sheet<T>::add(a, n->rc);
            DEBUG_VALUE_OF(n->rc);
            SDL_BlitSurface(tmp, NULL, surface, &n->rc);
            SDL_FreeSurface(tmp);
            
        }
        this->_texture = wee::assets<SDL_Texture>::instance().from_surface(_name, surface);

        SDL_FreeSurface(surface);
    }
};


typedef basic_sprite_font<char> sprite_font;


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

void foo() {
}

#include <engine/application.hpp>
#include <engine/applet.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
struct game : wee::applet {
    sprite_font* _sf;

    game() {
        if(!TTF_WasInit()) {
            TTF_Init();
        }
    }

    virtual ~game() {

        TTF_Quit();
    }


    int load_content() {

        wee::asset_helper::from_file<TTF_Font>("@debug", "assets/ttf/pzim3x5.ttf");

        //std::ifstream is(wee::get_resource_path("assets/ttf") + "pzim3x5.ttf");

        std::string pt = wee::get_resource_path("assets") + "ttf/BlackCastleMF.ttf";//Boxy-Bold.ttf";
        //std::string pt = wee::get_resource_path("assets") + "ttf/Boxy-Bold.ttf";
        std::ifstream is;
        is.open(pt);
        if(!is.is_open()) {
            throw ::file_not_found(pt);
        }
        std::istreambuf_iterator<char> eos;
        std::string contents(std::istreambuf_iterator<char>(is),
            (std::istreambuf_iterator<char>())
        );

        SDL_RWops* rw = SDL_RWFromConstMem(contents.c_str(), (int)contents.length());

        TTF_Font* font = TTF_OpenFontRW(rw, 0, 32);

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
            SDL_Rect src = *_sf->get(c);
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

        foo();

        return app.start();
    } catch(const std::exception& e) {
        LOGE(e.what());
    }

    return 0;
}
