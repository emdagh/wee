#include <wee/wee.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <engine/assets.hpp>
#include <util/logstream.hpp>
#include <nlohmann/json.hpp>
//std::map<uint16_t, SDL_Rect> index;
//

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
        node* left, *right;
        int id;
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
struct sprite_font : sprite_sheet<T> {

    TTF_Font* _font;
    std::string _name;
    font_info _info;
    std::map<T, glyph_info> _ginfo;

    void _texture_size(int n, int maxw, int maxh, int* w) {
        float a = n * maxw * maxh; 
        float x = std::sqrt(a);// * maxw > maxh ? maxw : maxh;
        x *= std::max(maxw, maxh);
        float d = std::round(x); 
        *w = math::npot(((int)d));
    }

    sprite_font(const std::string& name, TTF_Font* font) 
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
        for(unsigned char a=32; a < 255; a++) {
            TTF_GlyphSize(_font, (uint16_t)a, &w, &h);
            maxw = std::max(w, maxw);
            maxh = std::max(h, maxh);
        }

        int d = 0;
        _texture_size(255-32, maxw, maxh, &d);

        SDL_Surface* surface = SDL_CreateRGBSurface(0, d, d, 32,
                0, 0, 0, 0);

        
        packer::node* root = new packer::node;
        root->id = 0;
        root->left = root->right = NULL;
        root->rc = { 0, 0, d, d };
        for(unsigned char a=32; a < 255; a++) {
            //TTF_GlyphSize(_font, (uint16_t)a, &n->rc.w, &n->rc.h, NULL);
            glyph_info gi;
            TTF_GlyphMetrics(_font, (uint16_t)a,
                    &gi.minx,
                    &gi.maxx,
                    &gi.miny,
                    &gi.maxy,
                    &gi.advance);
            _ginfo[a] = gi;

            SDL_Surface* tmp = TTF_RenderGlyph_Blended(_font, a, {0xff, 0xff, 0xff, 0xff});
            packer::node* n = packer::insert(root, {0, 0, tmp->w, tmp->h});
            n->id = (int)a;

            sprite_sheet<T>::add(a, n->rc);
            SDL_BlitSurface(tmp, NULL, surface, &n->rc);
            SDL_FreeSurface(tmp);
            
        }
        wee::assets<SDL_Texture>::instance().from_surface(_name, surface);

        SDL_FreeSurface(surface);
    }
};





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
    if(!TTF_WasInit()) {
        TTF_Init();
    }

	std::string pt = wee::get_resource_path("assets") + "ttf/Boxy-Bold.ttf";
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

    TTF_Font* font = TTF_OpenFontRW(rw, 0, 16);

    sprite_font<int>* sf = new sprite_font<int>("@foofont", font);


    TTF_Quit();
}

int main(int, char* []) {
	
	try {
        foo();
    } catch(const std::exception& e) {
        LOGE(e.what());
    }

    return 0;
}
