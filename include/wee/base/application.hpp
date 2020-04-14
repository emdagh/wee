#pragma once

#include <wee/wee.h>
#include <core/delegate.hpp>

struct SDL_Application;

namespace wee {
    class graphics_initializer;
    class graphics_device;
    class applet;

    struct application_base {
        virtual ~application_base() {}
        virtual int start() = 0;
        virtual void resize(int, int) = 0;
        virtual void set_mouse_position(int, int) = 0;
        virtual void set_applet(applet*) = 0;
        
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

    class application : public application_base {
    protected:
        SDL_Application* _handle;
        applet* _applet;
        graphics_device* _graphics_device = nullptr;
        graphics_initializer&& _graphics_initializer;
    public:
        explicit application(applet*);
        application(applet*, graphics_initializer&&);
        virtual ~application();
        int start();
        //int stop(); 
        void resize(int w, int h);
        void set_mouse_position(int x, int y);
        void set_applet(applet*);
    public:
    };
}

C_API int application_create(wee::application_base**);
C_API int application_destroy(wee::application_base*);
//C_API int application_resize(wee_application_base*, int, int);
