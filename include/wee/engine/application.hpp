#pragma once

#include <core/delegate.hpp>

struct SDL_Application;

namespace wee {


    class applet;
    class application {
        SDL_Application* _handle;
        applet* _applet;
    public:
        explicit application(applet*);
        virtual ~application();
        int start();
        //int stop(); 
        void resize(int w, int h); 

    public:
        event_handler<int(uint16_t)> on_keypress;
        event_handler<int(uint16_t)> on_keyrelease;

        event_handler<int(uint16_t)> on_mousepress;
        event_handler<int(uint16_t)> on_mouserelease;
        event_handler<int(int, int)> on_mousemove;
    };

}
