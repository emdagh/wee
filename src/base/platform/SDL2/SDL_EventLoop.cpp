#include <util/logstream.hpp>
#include <base/platform/SDL2/SDL_EventLoop.hpp>
#include <SDL.h>
#include <iostream>

using namespace wee;
using namespace wee::detail;

SDL_EventLoop::SDL_EventLoop(fn_conditional a, fn_unconditional b) noexcept {
    DEBUG_METHOD();
    _conditional = a; 
    _unconditional = b;
}

int SDL_EventLoop::start() {
    try {
        SDL_Event event;
        while(true) {
            while(SDL_PollEvent(&event)) {
                if(_conditional(event)) {
                    return 0;
                }
            }
            // draw functionality typically goes here.
            _unconditional(); 
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
