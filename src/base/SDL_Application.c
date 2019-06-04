#include <base/SDL_Application.h>
#include <SDL.h>
#include <string.h>
#include <weegl.h>

#define DELEGATE(x, arg_0, arg_1) \
    return ptr->callback[x] ? ptr->callback[x](arg_0, (const void*)arg_1) : 0

typedef int(*SDL_ConditionalEvent)(const SDL_Event*);
typedef void(*SDL_UnconditionalEvent)(const void*);

typedef struct SDL_Application{
    SDL_Window*     window;
    SDL_Renderer*   renderer;
    SDL_ApplicationCallback callback[SDL_APPLICATION_CALLBACK_MAX];
    SDL_Texture* fb;
    int is_running;
    int is_paused;
    void* userdata;


} SDL_Application;


void SDL_SetApplicationUserData(struct SDL_Application* ptr, const void* userdata) {
    ptr->userdata = userdata;
}

void* SDL_GetApplicationUserData(const struct SDL_Application* ptr) {
    return ptr->userdata;
}


int SDL_ApplicationHandleWindowEvent(struct SDL_Application* ptr, const SDL_WindowEvent* e) {
    switch(e->event) {
        case SDL_WINDOWEVENT_SHOWN:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window shown");
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window hidden");
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window exposed");
            break;
        case SDL_WINDOWEVENT_MOVED:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window moved");
//                e->data1,
//                e->data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            DELEGATE(SDL_APPLICATION_CALLBACK_WINDOW_RESIZE, ptr, e);
//                    e->data1,
//                    e->data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            //DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window size changed");// (%d, %d)",
            DELEGATE(SDL_APPLICATION_CALLBACK_WINDOW_RESIZE, ptr, e);
//                    e->data1,
//                    e->data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window minimized");
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window minimized");
            break;
        case SDL_WINDOWEVENT_RESTORED:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window restored");
            break;
        case SDL_WINDOWEVENT_ENTER:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window entered");
            break;
        case SDL_WINDOWEVENT_LEAVE:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window left");
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window focus acquired");
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window focus lost");
            break;
        case SDL_WINDOWEVENT_CLOSE:
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Window closed");
            break;
        default:
            SDL_Log("Unknown window event");
            DELEGATE(SDL_APPLICATION_CALLBACK_LOG, ptr, "Unknown window event");
            break;
    }
    return 0;
}


int SDL_ApplicationHandleTextEditingEvent(struct SDL_Application* ptr, const SDL_TextEditingEvent* e) {
    return 0;
}
int SDL_ApplicationHandleKeyboardEvent(struct SDL_Application* ptr, const SDL_KeyboardEvent* e) {
    DELEGATE(SDL_APPLICATION_CALLBACK_KEY, ptr, e);
}
int SDL_ApplicationHandleGestureEvent(struct SDL_Application* ptr, const SDL_MultiGestureEvent* e) {
    return 0;
}
int SDL_ApplicationHandleMouseMotionEvent(struct SDL_Application* ptr, const SDL_MouseMotionEvent* e) {
    DELEGATE(SDL_APPLICATION_CALLBACK_MOUSEMOVE, ptr, e);
}
int SDL_ApplicationHandleFingerEvent(struct SDL_Application* ptr, const SDL_TouchFingerEvent* e) {
    return 0;
}
int SDL_ApplicationHandleMouseButtonEvent(struct SDL_Application* ptr, const SDL_MouseButtonEvent* e) {
    DELEGATE(SDL_APPLICATION_CALLBACK_MOUSEBUTTON, ptr, e);
}
int SDL_ApplicationHandleTextInputEvent(struct SDL_Application* ptr, const SDL_TextInputEvent* e) {
    return 0;
}


struct SDL_Application* SDL_CreateApplication() { 
    if(SDL_Init(SDL_INIT_VIDEO)) {
        return NULL;
    }
    struct SDL_Application* res = (struct SDL_Application*)malloc(sizeof(SDL_Application));
    memset(res, 0, sizeof(SDL_Application));
    return res;
}

int SDL_InitApplication(SDL_Application* res) {
    /**
     * create the window
     */
    {
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        SDL_Window* _win = SDL_CreateWindow(NULL, 
                0, 0, 
                640, //dm.w, 
                480, //dm.h, 
                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI 
//#if defined(IOS) || defined(ANDROID)
//                | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS
//#else
                | SDL_WINDOW_RESIZABLE
//#endif
                );

        res->window = _win;
    }

    if(!res->window) 
        return -1;
    /**
     * create the renderer
     */
    {
        SDL_GL_CreateContext(res->window);

        SDL_Renderer* _ren = SDL_CreateRenderer(res->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
        );
        if (!_ren){
            SDL_DestroyWindow(res->window);
            //SDL_Quit();
        }



        res->renderer = _ren;
    }
    if(!res->renderer) 
        return -1;


    if(res->callback[SDL_APPLICATION_CALLBACK_CREATED]) 
        res->callback[SDL_APPLICATION_CALLBACK_CREATED](res, NULL);

}

int SDL_DestroyApplication(SDL_Application* ptr) {

	return 0;
}
struct SDL_Window* SDL_GetApplicationWindow(const SDL_Application* ptr) {
    return ptr->window;
}

struct SDL_Renderer* SDL_GetApplicationRenderer(const SDL_Application* ptr) {
    return ptr->renderer;
}
void SDL_SetApplicationCallback(SDL_Application* ptr, SDL_ApplicationCallbackType type, SDL_ApplicationCallback fn) {
    ptr->callback[type] = fn;
}
int SDL_ApplicationHandleEvent(SDL_Application* ptr, const SDL_Event* ev) {

    switch(ev->type) {
        case SDL_QUIT: {
                           SDL_Quit();
                           return 1;
                       }
        case SDL_WINDOWEVENT:
                       return SDL_ApplicationHandleWindowEvent(ptr, &ev->window);
        case SDL_KEYDOWN:
        case SDL_KEYUP:
                       return SDL_ApplicationHandleKeyboardEvent(ptr, &ev->key);
        case SDL_TEXTINPUT:
                       return SDL_ApplicationHandleTextInputEvent(ptr, &ev->text);
        case SDL_TEXTEDITING:
                       return SDL_ApplicationHandleTextEditingEvent(ptr, &ev->edit);
        case SDL_MULTIGESTURE:
                       return SDL_ApplicationHandleGestureEvent(ptr, &ev->mgesture);
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:
                       return SDL_ApplicationHandleFingerEvent(ptr, &ev->tfinger);
        case SDL_MOUSEMOTION:
                       return SDL_ApplicationHandleMouseMotionEvent(ptr, &ev->motion);
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
                       return SDL_ApplicationHandleMouseButtonEvent(ptr, &ev->button);
    }
    return 0;
}

int SDL_StartApplication(struct SDL_Application* ptr) {

    ptr->is_running = 1;

    if(ptr->callback[SDL_APPLICATION_CALLBACK_STARTED]) 
        ptr->callback[SDL_APPLICATION_CALLBACK_STARTED](ptr, NULL);

    int dt = 1000 / 60;
    int timeCurrentMs = 0;
    int timeAccumulatedMs = 0;
    int timeDeltaMs = 0;
    int timeLastMs = SDL_GetTicks();

    SDL_Event event;
    while(ptr->is_running) {
        if(ptr->is_paused)
            continue;

        while(SDL_PollEvent(&event)) {
            if(SDL_ApplicationHandleEvent(ptr, &event)) {
                return 0;
            }
        }
        timeCurrentMs = SDL_GetTicks();
        timeDeltaMs = timeCurrentMs - timeLastMs;
        timeAccumulatedMs += timeDeltaMs;
        while(timeAccumulatedMs >= dt) {
            ptr->callback[SDL_APPLICATION_CALLBACK_UPDATE](ptr, &dt);
            timeAccumulatedMs -= dt;
        }

        ptr->callback[SDL_APPLICATION_CALLBACK_RENDER](ptr, NULL);
        timeLastMs = timeCurrentMs;
        SDL_GL_SwapWindow(ptr->window);
    }
}

void SDL_StopApplication(struct SDL_Application* ptr) {
    //SDL_StopEventLoop(ptr->events);
    ptr->is_running = 0;
    ptr->callback[SDL_APPLICATION_CALLBACK_STOPPED](ptr, NULL);
}
void SDL_PauseApplication(struct SDL_Application* ptr) {

}
void SDL_ResumeApplication(struct SDL_Application* ptr) {
}

