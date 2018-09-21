#pragma once

#include <engine/sprite_sheet.hpp>
#include <engine/assets.hpp>
#include <engine/packer.hpp>

#include <map>
#include <string>
#include <cmath>
#include <SDL.h>
#include <SDL_ttf.h>

int TTF_GlyphSize(TTF_Font* font, Uint16 c, int* w, int* h);

namespace wee {
    
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

    template <typename T>
    struct basic_sprite_font : public wee::basic_sprite_sheet<T> {

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

                this->add(a, n->rc);

                SDL_BlitSurface(tmp, NULL, surface, &n->rc);
                SDL_FreeSurface(tmp);
                
            }
            this->_texture = wee::assets<SDL_Texture>::instance().from_surface(_name, surface);

            SDL_FreeSurface(surface);
        }
    };


    typedef basic_sprite_font<char> sprite_font;
}
