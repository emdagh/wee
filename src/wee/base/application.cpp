#include <weegl.h>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/assets.hpp>
#include <base/platform/SDL/SDL_Application.h>
#include <core/logstream.hpp>
#include <gfx/graphics_device.hpp>
#include <gfx/graphics_initializer.hpp>

using namespace wee;

// During init, enable debug output

graphics_initializer default_graphics_initializer;

application::application(applet* a) 
: application(a, default_graphics_initializer)
{

}

application::application(applet* a, const graphics_initializer& init) 
    : _handle(SDL_CreateApplication())
    , _graphics_initializer(init) //std::forward<graphics_initializer>(init))

{
    set_applet(a);

}

application::~application() {
}

void application::resize(int w, int h) {
    SDL_SetWindowSize(SDL_GetApplicationWindow(_handle), w, h);
    SDL_RenderSetLogicalSize(SDL_GetApplicationRenderer(_handle), w, h);
}

void application::set_mouse_position(int x, int y) {
    SDL_WarpMouseInWindow(SDL_GetApplicationWindow(_handle), x, y);
}

int application::start() {
    return SDL_StartApplication(_handle);
}
void application::set_applet(applet* arg) {
    _applet = arg;
    if(!_handle) {
        DEBUG_LOG(SDL_GetError());
        SDL_Quit();
    }
    /**
     * note that event.keysym.sym will return symbolic key values so: on azerty, pressing 'a' will result in 'a', whereas event.keysym.scancode would return 'q'
     * On AZERTY, pressing 'A' will emit 'Q' scancode and 'a' keysym
     */
    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_KEY, [] (const SDL_Application* app, const void* userdata) {
        application* a = static_cast<application*>(SDL_GetApplicationUserData(app));
        const SDL_KeyboardEvent* event = static_cast<const SDL_KeyboardEvent*>(userdata);
        if(event->type == SDL_KEYUP) {
            a->on_keyrelease(event->keysym.sym);
        } else if(event->type == SDL_KEYDOWN) {
            a->on_keypress(event->keysym.sym);
        } else {
            return -1;
        }
        return 0;
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_LOG, [] (const SDL_Application* app, const void* data) {
        const char* str = reinterpret_cast<const char*>(data);
        DEBUG_LOG(str);
        return 0;
    });
    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_CREATED, [] (const SDL_Application* app, const void* ) {

        application* a = static_cast<application*>(SDL_GetApplicationUserData(app));
        a->_graphics_device = new graphics_device(SDL_GetApplicationRenderer(app));
		return 0;
		//return make_graphics_device(&a->_graphics_device);            
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_STARTED, [] (const SDL_Application* app, const void* ) {
        application* a = static_cast<application*>(SDL_GetApplicationUserData(app));
        int w, h;
        SDL_GetWindowSize(SDL_GetApplicationWindow(app), &w, &h);
        a->on_resize(w, h); // < here
        return a->_applet->load_content();
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_MOUSEMOVE, [] (const SDL_Application* app, const void* userdata) {
        const SDL_MouseMotionEvent* event = static_cast<const SDL_MouseMotionEvent*>(userdata);
        application* self = static_cast<application*>(SDL_GetApplicationUserData(app));
        self->on_mousemove(event->x, event->y);
        return 0;
    });
    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_WINDOW_RESIZE, [] (const SDL_Application* app, const void* data) {
        const SDL_WindowEvent* e = static_cast<const SDL_WindowEvent*>(data);
        auto* self = static_cast<application*>(SDL_GetApplicationUserData(app));
        self->on_resize(e->data1, e->data2);
        return 0;
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_MOUSEBUTTON, [] (const SDL_Application* app, const void* userdata) {
        const SDL_MouseButtonEvent* event = static_cast<const SDL_MouseButtonEvent*>(userdata);
        application* self = static_cast<application*>(SDL_GetApplicationUserData(app));
        if(event->type == SDL_MOUSEBUTTONDOWN) {
            self->on_mousedown(event->button);
        } else if(event->type == SDL_MOUSEBUTTONUP) {
            self->on_mouseup(event->button);
        }
        return 0;
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_UPDATE, [] (const SDL_Application* app, const void* userdata) {
        application* _ = static_cast<application*>(SDL_GetApplicationUserData(app));
        return _->_applet->update(*(static_cast<const int*>(userdata)));
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_RENDER, [] (const SDL_Application* app, const void* ) {
        application* self = static_cast<application*>(SDL_GetApplicationUserData(app));
        //graphics_device* device = nullptr;
        //SDL_Renderer* renderer = SDL_GetApplicationRenderer(app);
        //glViewport(0, 0, w, h);
        return self->_applet->draw(self->_graphics_device);
    });


    SDL_SetApplicationUserData(_handle, this);
    SDL_InitApplication(_handle, 
        _graphics_initializer.width(), 
        _graphics_initializer.height(), 
        _graphics_initializer.depth_bits(), 
        _graphics_initializer.stencil_bits(), 
        _graphics_initializer.vsync()
    );


    int w, h;
    SDL_GetWindowSize(SDL_GetApplicationWindow(_handle), &w, &h);
    SDL_RenderSetLogicalSize(SDL_GetApplicationRenderer(_handle), w, h);
    assets<SDL_Texture>::instance().after= [&] (SDL_Surface* surface) {
        return SDL_CreateTextureFromSurface(SDL_GetApplicationRenderer(_handle), surface);
    };
}
