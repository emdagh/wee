#ifndef _SDL_APPLICATION_H_
#define _SDL_APPLICATION_H_

#include <wee/wee.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Application;
struct SDL_EventLoop;

typedef int(*SDL_ApplicationCallback)(const struct SDL_Application*, const void*);

typedef enum {
    SDL_APPLICATION_CALLBACK_CREATED,
    SDL_APPLICATION_CALLBACK_DESTROYED,
    SDL_APPLICATION_CALLBACK_STARTED,
    SDL_APPLICATION_CALLBACK_STOPPED,
    SDL_APPLICATION_CALLBACK_PAUSED,
    SDL_APPLICATION_CALLBACK_RESUMED,
    SDL_APPLICATION_CALLBACK_UPDATE,
    SDL_APPLICATION_CALLBACK_RENDER,
    SDL_APPLICATION_CALLBACK_MOUSEMOVE,
    SDL_APPLICATION_CALLBACK_MOUSEBUTTON,
    SDL_APPLICATION_CALLBACK_WINDOW_RESIZE,
    SDL_APPLICATION_CALLBACK_LOG,

    SDL_APPLICATION_CALLBACK_KEY,

    SDL_APPLICATION_CALLBACK_EVENTS_CONDITIONAL,
    SDL_APPLICATION_CALLBACK_EVENTS_UNCONDITIONAL,

    SDL_APPLICATION_CALLBACK_MAX

} SDL_ApplicationCallbackType;


APICALL struct SDL_Application* SDL_CreateApplication();
APICALL int SDL_InitApplication(struct SDL_Application*);
APICALL int SDL_DestroyApplication(struct SDL_Application*);
APICALL struct SDL_Window* SDL_GetApplicationWindow(const struct SDL_Application*);
APICALL struct SDL_Renderer* SDL_GetApplicationRenderer(const struct SDL_Application*);
APICALL void SDL_SetApplicationCallback(struct SDL_Application*, SDL_ApplicationCallbackType, SDL_ApplicationCallback);


APICALL int SDL_StartApplication(struct SDL_Application*);
APICALL void SDL_StopApplication(struct SDL_Application*);
APICALL void SDL_PauseApplication(struct SDL_Application*);
APICALL void SDL_ResumeApplication(struct SDL_Application*);
APICALL void SDL_SetApplicationUserData(struct SDL_Application*, const void*);
APICALL void* SDL_GetApplicationUserData(const struct SDL_Application*);

#endif
