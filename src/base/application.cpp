#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/assets.hpp>
#include <base/SDL_Application.h>
#include <core/logstream.hpp>

using namespace wee;

application::application(applet* a) 
    : _handle(SDL_CreateApplication())
    , _applet(a)

{
    if(!_handle) {
        DEBUG_LOG(SDL_GetError());
    }
    assets<SDL_Texture>::instance().after= [&] (SDL_Surface* surface) {
        return SDL_CreateTextureFromSurface(SDL_GetApplicationRenderer(_handle), surface);
    };

    SDL_SetApplicationUserData(_handle, this);
    int w, h;
    SDL_GetWindowSize(SDL_GetApplicationWindow(_handle), &w, &h);
    SDL_RenderSetLogicalSize(SDL_GetApplicationRenderer(_handle), w, h);

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_STARTED, [] (const SDL_Application* app, const void* ) {
        application* a = static_cast<application*>(SDL_GetApplicationUserData(app));
        return a->_applet->load_content();
    });

    SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_MOUSEMOVE, [] (const SDL_Application* app, const void* userdata) {
        const SDL_MouseMotionEvent* event = static_cast<const SDL_MouseMotionEvent*>(userdata);
        application* self = static_cast<application*>(SDL_GetApplicationUserData(app));
        self->on_mousemove(event->x, event->y);
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
        SDL_Renderer* renderer = SDL_GetApplicationRenderer(app);
        return self->_applet->draw(renderer);
    });
}

application::~application() {
}

void application::resize(int w, int h) {
    SDL_SetWindowSize(SDL_GetApplicationWindow(_handle), w, h);
    SDL_RenderSetLogicalSize(SDL_GetApplicationRenderer(_handle), w, h);
}

int application::start() {
    return SDL_StartApplication(_handle);
}
