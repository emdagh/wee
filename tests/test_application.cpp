#include <util/logstream.hpp>
#include <base/SDL_Application.h>
#include <base/SDL_ColorEXT.hpp>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace wee;


APICALL void SDL_SetRenderDrawColorEXT(SDL_Renderer* renderer, const SDL_Color* color) {
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}


class applet {
    public:
        virtual int update(int) = 0;
        virtual int draw() = 0;
};

class application { 
    SDL_Application* _handle;
    applet* _applet;

    public:
    application() 
        : _handle(SDL_CreateApplication())
    {
        DEBUG_METHOD();
        if(!_handle) {
            DEBUG_LOG(SDL_GetError());
        }

        SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_MOUSEMOVE, [&] (const SDL_Application* app, const void* userdata) {
                const SDL_MouseMotionEvent* event = static_cast<const SDL_MouseMotionEvent*>(userdata);


                SDL_GetApplicationWindow(app);
                //DEBUG_LOG("mouse motion {0},{1}", event->x, event->y);
                //
                int sx, sy;
                int rw, rh;
                int w, h;

                SDL_GetWindowSize(SDL_GetApplicationWindow(app), &w, &h);
                SDL_RenderGetLogicalSize(SDL_GetApplicationRenderer(app), &rw, &rh);

                DEBUG_LOG("window size=[{}x{}]", w, h);
                DEBUG_LOG("renderer size=[{}x{}]", rw, rh);

                return 0;

                });

        SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_UPDATE, [&] (const SDL_Application* app, const void* userdata) {
                static int once = 0;
                if(!once) {
                    once = 1;
                    SDL_Window* window = SDL_GetApplicationWindow(app);
                    SDL_Renderer* renderer = SDL_GetApplicationRenderer(app);
                    //SDL_SetWindowSize(window, 640, 480);
                    SDL_RenderSetLogicalSize(renderer, 1024, 1024);
                }
                return 0;
                });




        SDL_SetApplicationCallback(_handle, SDL_APPLICATION_CALLBACK_RENDER, [&] (const SDL_Application* app, const void* userdata) {
                SDL_Renderer* renderer = SDL_GetApplicationRenderer(app);
                static SDL_Texture* fb = NULL;
                static SDL_Texture* texture = NULL;

                static int once = 0;
                if(!once) {
					SDL_Surface* temp = IMG_Load("awesome_face.png");
					texture = SDL_CreateTextureFromSurface(renderer, temp);

                    fb = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
                    once = 1;
                }

                /**
                 * render the scene 
                 */
                {
                    SDL_SetRenderTarget(renderer, fb);
                    SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);//IndianRed);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                }


                SDL_SetRenderTarget(renderer, NULL);
                SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
                SDL_RenderClear(renderer);
                SDL_RenderCopyEx(renderer, fb, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
                SDL_RenderPresent(renderer);
                return 0;
                });
    }

    int start() {
        DEBUG_METHOD();
        return SDL_StartApplication(_handle);
    }
};

int main(int argc, char* argv[]) {
    DEBUG_METHOD();
    try {
        application app;
        return app.start();
    } catch(...) {
        std::cout << "exception caught" << std::endl;
    }
}

static class SDL_Initializer {
    public:
        SDL_Initializer() { 
            DEBUG_METHOD();
            SDL_Init(SDL_INIT_EVERYTHING);
        }

        virtual ~SDL_Initializer() {
            DEBUG_METHOD();
            SDL_Quit();
        }
} _;
