#pragma once

#include <wee/wee.hpp>
#include <SDL.h>
#include <vector>
#include <algorithm>
#include <core/rect.hpp>
#include <core/vec2.hpp>

namespace wee {

    struct layout {
        /**
         * add a sub-rectangle to the current layout
         */
        virtual void add(rect*) = 0;
        /**
         * apply the layout to all subrect within the context of 
         * a parent rect
         */
        virtual void apply(const rect&) = 0;
    };

    struct flow_layout : layout {
        enum alignment {
            CENTER,
            LEADING,
            LEFT,
            RIGHT,
            TRAILING,
            ALIGNMENT_MAX
        };

        alignment _a = alignment::CENTER;
        std::vector<rect*> _r;


        virtual void add(rect* r) {
            _r.push_back(r);
        }

        virtual void apply(const rect& parent) {
            int y = parent.y;

            std::vector<rect*> row;
            vec2i rowdim = { 0, 0 };

            for(auto* r : _r) {
                row.push_back(r);
                rowdim.y = std::max(rowdim.y, r->h);
                rowdim.x += r->w;

                if(rowdim.x > parent.w) {
                    align(row, y, rowdim, parent);
                    row.clear();

                    y += rowdim.y;
                    rowdim.x = rowdim.y = 0;
                }
            }
            align(row, y, rowdim, parent);
        }

        void align(const std::vector<rect*>& row,
                int y, 
                const vec2i& rowdim,
                const rect& parent) {
            vec2i location = {
                0, 0
            };

            switch(_a) {
                case alignment::CENTER:
                    location.x = (parent.x - rowdim.x) / 2;
                    break;
                case alignment::LEFT:
                    location.x = 0;
                    break;
                case alignment::RIGHT:
                    location.x = (parent.x - rowdim.x);
                    break;
                default:
                    break;
            }

            for(auto* i : row) {
                location.y = y + (parent.y / 2) - (i->h / 2);
                i->x = location.x;
                i->y = location.y;
                location.x += i->w;
            }
        }
    };

    struct border_layout : layout {

        enum location{
            PAGE_START,
            LINE_END,
            PAGE_END,
            LINE_START,
            CENTER,
            MAX_LOCATION
        };

        union {
            struct {
                rect* north, *east, *south, *west, *center; 
            };
            rect* _r[location::MAX_LOCATION];
        };


        virtual void add(rect* rc, const location ix) {
            _r[ix] = rc;
        }

        virtual void add(rect*) {
            throw not_implemented();
        }

        void set_bounds(rect* r, int x, int y, int w, int h) {
            r->x = x;
            r->y = y;
            r->w = w;
            r->h = h;
        }

        const rect* get(const location ix) {
            return _r[ix];
        }

        virtual void apply(const rect& parent) {
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
