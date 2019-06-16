#include <core/logstream.hpp>
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


    ms dt(60);

    auto timeLastMs = timer::now();
    auto timeAccumulatedMs = dt.count();

    try {
        SDL_Event event;
        while(true) {
            while(SDL_PollEvent(&event)) {
                if(_conditional(event)) {
                    return 0;
                }
            }
            // draw functionality typically goes here.
            timeAccumulatedMs += (timer::now() - timeLastMs).count();

            _unconditional(&timeAccumulatedMs, dt.count()); 

            timeLastMs = timer::now();

        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
