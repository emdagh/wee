#pragma once

#include <wee/wee.hpp>
#include <SDL.h>

namespace wee {

    struct layout {
        /**
         * add a sub-rectangle to the current layout
         */
        virtual void add(SDL_Rect*) = 0;
        /**
         * apply the layout to all subrect within the context of 
         * a parent rect
         */
        virtual void apply(const SDL_Rect&) = 0;
    };

    struct border_layout : layout {

        enum location{
            NORTH,
            EAST,
            SOUTH,
            WEST,
            CENTER,
            MAX_LOCATION
        };

        union {
            struct {
                SDL_Rect* north, *east, *south, *west, *center; 
            };
            SDL_Rect* _r[location::MAX_LOCATION];
        };


        virtual void add(SDL_Rect* rc, const location ix) {
            _r[ix] = rc;
        }

        virtual void add(SDL_Rect* rc) {
            throw not_implemented();
        }

        void set_bounds(SDL_Rect* r, int x, int y, int w, int h) {
            r->x = x;
            r->y = y;
            r->w = w;
            r->h = h;
        }

        virtual void apply(const SDL_Rect& parent) {
            int top, bottom, left, right;
            top = parent.y;
            bottom = top + parent.h;
            left = parent.x;
            right = left + parent.w;

            if(north) {
                north->x = left;
                north->y = top;
                north->w = right - left;
                top += north->h;
            }

            if(south) {
                south->x = left;
                south->y = bottom - south->h;
                south->w = right - left;
                bottom -= south->h;
            }

            if(east) {
                east->h = bottom - top;
                east->x = right - east->w;
                east->y = top;
                right -= east->w;
            }

            if(west) {
                west->h = bottom - top;
                west->x = left;
                west->y = top;
                left += west->w;
            }

            if(center) {
                center->x = left;
                center->y = top;
                center->w = right - left;
                center->h = bottom - top;
            }
        }
    };
}
