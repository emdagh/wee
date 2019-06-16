#include <engine/packer.hpp>
#include <nlohmann/json.hpp>
#include <SDL.h>


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
