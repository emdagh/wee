#pragma once

#include <core/math.hpp>
#include <functional>
#include <SDL.h>

std::ostream& operator << (std::ostream& os, const SDL_Rect& rc);

namespace packer {
    constexpr static const int INVALID = -1;

    struct node {
        node* left = nullptr, *right = nullptr;
        int id = -1;
        SDL_Rect rc;
    };

    void iterate(node* n, std::function<void(int, const SDL_Rect&)> fn);
    node* insert(node* n, const SDL_Rect& rc);
    
}
