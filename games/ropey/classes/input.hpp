#pragma once

#include <core/singleton.hpp>
/**
 * it's  a bit of a weak move. But this seems like the shortest blow 
 */
struct input : wee::singleton<input> {
    bool mouse_down;
    int mouse_x, mouse_y;    
};
