#pragma once

#include <core/delegate.hpp>

struct SDL_Application;

namespace wee {

    class graphics_device;
    class applet;
    class application {
        SDL_Application* _handle;
        applet* _applet;
        graphics_device* _graphics_device = nullptr;
    public:
        explicit application(applet*);
        virtual ~application();
        int start();
        //int stop(); 
        void resize(int w, int h);
        void set_mouse_position(int x, int y);
    protected:
        int get_graphics_device(graphics_device**);

    public:
        event_handler<int(int, int)> on_resize;
        event_handler<int(uint16_t)> on_keypress;
        event_handler<int(uint16_t)> on_keyrelease;

        event_handler<int(uint8_t)> on_mousedown;
        event_handler<int(uint8_t)> on_mouseup;
        event_handler<int(int, int)> on_mousemove;

        event_handler<void(void)> before_update;
        event_handler<void(void)> after_update;
        event_handler<void(void)> before_draw;
        event_handler<void(void)> after_draw;
    };

}
