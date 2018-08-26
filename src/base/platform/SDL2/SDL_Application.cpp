#include <util/logstream.hpp>
#include <base/platform/SDL2/SDL_Application.hpp>
#include <base/platform/SDL2/SDL_EventLoop.hpp>
#include <SDL.h>

namespace wee {
    namespace detail {


        struct SDL_Application::impl {
            SDL_EventLoop*  _events;
            SDL_Window*     _window;
            SDL_Renderer*   _renderer;

            
            void internal_WindowEvent(const SDL_WindowEvent& window) {
                DEBUG_METHOD();
                switch(window.event) {
                    case SDL_WINDOWEVENT_SHOWN:
                        DEBUG_LOG("Window {0} shown", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        DEBUG_LOG("Window {} hidden", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        DEBUG_LOG("Window {} exposed", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_MOVED:
                        DEBUG_LOG("Window {} moved to {},{}",
                                window.windowID, window.data1,
                                window.data2);
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        DEBUG_LOG("Window {} resized to {}x{}",
                                window.windowID, window.data1,
                                window.data2);
                        //resize(window.data1, window.data2);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        DEBUG_LOG("Window {} size changed to {}x{}",
                                window.windowID, window.data1,
                                window.data2);
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        DEBUG_LOG("Window {} minimized", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        DEBUG_LOG("Window {} maximized", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        DEBUG_LOG("Window {} restored", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                        DEBUG_LOG("Mouse entered window {}",
                                window.windowID);
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        DEBUG_LOG("Mouse left window {}", window.windowID);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        DEBUG_LOG("Window {} gained keyboard focus",
                                window.windowID);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        DEBUG_LOG("Window {0} lost keyboard focus",
                                window.windowID);
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        DEBUG_LOG("Window {} closed", window.windowID);
                        break;
                    default:
                        DEBUG_LOG("Window {0} got unknown event {1}",
                                window.windowID, window.event);
                        break;
                }
            }

            void internal_MouseButtonEvent(const SDL_MouseButtonEvent& button) {
                //if(button.type == SDL_MOUSEBUTTONDOWN)
                    //OnMouseDown(button.x, button.y);
                //else 
                    //OnMouseRelease(button.x, button.y);
            }
    
    
            void internal_TextInputEvent(const SDL_TextInputEvent& ev) {
                std::cout << "text input: " << ev.text << std::endl;
                //OnTextInput(ev.text);
            }

            void internal_TextEditingEvent(const SDL_TextEditingEvent& ev) {
                std::cout << "text editing: " << ev.text << std::endl;
                //OnTextInput(ev.text);
            }

            void internal_KeyboardEvent(const SDL_KeyboardEvent& ev) {
                if(ev.state == SDL_PRESSED) {
                    //OnKeyPressed((uint16_t)ev.keysym.sym);
                }
            }

            void internal_GestureEvent(const SDL_MultiGestureEvent& ev) {
            }

            void internal_FingerEvent(const SDL_TouchFingerEvent& ev) 
            {
                SDL_Finger* finger = SDL_GetTouchFinger(ev.touchId, 0);
            }

            void internal_MouseMotionEvent(const SDL_MouseMotionEvent& motion) {
                //OnMouseMove(motion.x, motion.y, motion.xrel, motion.yrel);
            }
            
            int _input(SDL_Application* app, const SDL_Event& ev) {
                switch(ev.type) {
                    case SDL_QUIT: {
                        DEBUG_METHOD();
                        DEBUG_LOG("SDL_QUIT");
                        SDL_Quit();
                        return 1;
                    }
                    case SDL_WINDOWEVENT:
                        internal_WindowEvent(ev.window);
                        break;
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                        internal_KeyboardEvent(ev.key);
                        break;
                    case SDL_TEXTINPUT:
                        internal_TextInputEvent(ev.text);
                        break;
                    case SDL_TEXTEDITING:
                        internal_TextEditingEvent(ev.edit);
                        break;
                    case SDL_MULTIGESTURE:
                        internal_GestureEvent(ev.mgesture);
                        break;
                    case SDL_FINGERDOWN:
                    case SDL_FINGERUP:
                    case SDL_FINGERMOTION:
                        internal_FingerEvent(ev.tfinger);
                        break;
                    case SDL_MOUSEMOTION:
                        internal_MouseMotionEvent(ev.motion);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                        internal_MouseButtonEvent(ev.button);
                        break;
                }
                return 0;
            }

            impl(SDL_Application* app) 
                : _events(new SDL_EventLoop(
                    [&] (const SDL_Event& ev) {
                        return this->_input(app, ev);
                    },
                    [&] (long int* acc, long int dt) {

                        while(*acc >= dt) {
                            *acc -= dt;
                            app->on_update(dt);
                        }
                        app->on_render();
                    }
                ))
            {
                DEBUG_METHOD();

                if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
                    //std::cout << "SDL_Init() Error: " << SDL_GetError() << std::endl;
                    DEBUG_VALUE_OF(SDL_GetError());
                    SDL_Quit();
                }
        
                SDL_DisplayMode dm;
                SDL_GetDesktopDisplayMode(0, &dm);
                int _width = dm.w;
                int _height= dm.h;
                SDL_Window* _win = SDL_CreateWindow(NULL, 0, 0, _width, _height, SDL_WINDOW_OPENGL
#if defined(ATOM_PLATFORM_IOS) | defined(ATOM_PLATFORM_ANDROID)
                        | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS
#else
                        | SDL_WINDOW_RESIZABLE
#endif
                        );

                if(!_win) {
                    DEBUG_VALUE_OF(SDL_GetError());
                    SDL_Quit();
                }

                DEBUG_VALUE_AND_TYPE_OF(_win);

                SDL_GL_CreateContext(_win);

                SDL_Renderer* _ren = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                if (_ren == nullptr){
                    SDL_DestroyWindow(_win);
                    SDL_Quit();
                }

                _events->start();
            }


        };
    }
}

using namespace wee;
using namespace wee::detail;


SDL_Application::SDL_Application() 
    : _pimpl(new impl(this))
{
}
SDL_Application::~SDL_Application() {
    if(_pimpl) {
        delete _pimpl;
        _pimpl = nullptr;
    }
}

int SDL_Application::create() {
    assert(_pimpl);
    return base_application::create();
}
int SDL_Application::start() {
    assert(_pimpl);
    return base_application::start();
}
int SDL_Application::resume() {
    assert(_pimpl);
    return base_application::resume();
}
int SDL_Application::pause(){
    assert(_pimpl);
    return base_application::pause();
}
int SDL_Application::stop() {
    assert(_pimpl);
    return base_application::stop();
}
int SDL_Application::destroy() {
    assert(_pimpl);
    return base_application::destroy();
}

APICALL int wee_create_application(wee::application** ptr) {
    if(*ptr != NULL) 
        return -1;
    *ptr = new wee::detail::SDL_Application;
    return 0;
}

APICALL int wee_destroy_application(wee::application* ptr) {
    delete ptr; 
    return 0;
}
