CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(SDL2 C)

set(SDL_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/SDL2/include"
)


add_definitions(-DSDL_GLES2_USE_VBOS -DGL_GLEXT_PROTOTYPES)


if(ATOM_PLATFORM_IOS)
    file(GLOB SDL_SOURCES_PLATFORM
    SDL2/src/video/uikit/*.m
    SDL2/src/video/dummy/*.c
    SDL2/src/timer/unix/*.c
    SDL2/src/thread/pthread/*.c
    SDL2/src/render/opengles/*.c
    SDL2/src/render/opengles2/*.c
    SDL2/src/render/software/*.c
    SDL2/src/power/uikit/*.m
    SDL2/src/loadso/dummy/*.c
    SDL2/src/loadso/dlopen/*.c
    SDL2/src/joystick/iphoneos/*.m
    SDL2/src/haptic/dummy/*.c
    SDL2/src/file/cocoa/*.m
    SDL2/src/filesystem/cocoa/*.m
    SDL2/src/audio/coreaudio/*.c
    SDL2/src/audio/dummy/*.c
    )
    #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-arc")
    set(SDL_COMPILE_FLAGS "-fobjc-arc -g -O0")

    
    include_directories(
        ${SDL_INCLUDE_DIRS}
    )
endif(ATOM_PLATFORM_IOS)
    
if(ATOM_PLATFORM_X11)
    add_subdirectory(SDL2)
    include_directories(
        #${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/vendor/sdl/include
        ${SDL_INCLUDE_DIRS}
    )
    message("SDL binary dir: ${CMAKE_CURRENT_BINARY_DIR}/vendor/sdl/include")
endif(ATOM_PLATFORM_X11)

if(ATOM_PLATFORM_ANDROID)
    #remove_definitions (-DSDL_VIDEO_RENDER_OGL_ES)
    add_definitions(-DGL_GLEXT_PROTOTYPES)
    file(GLOB SDL_SOURCES_PLATFORM
        SDL2/src/haptic/dummy/*.c
        SDL2/src/video/android/*.c
        SDL2/src/audio/android/*.c
        SDL2/src/audio/dummy/*.c
        SDL2/src/core/android/*.c
        SDL2/src/filesystem/android/*.c
        SDL2/src/joystick/android/*.c
        SDL2/src/loadso/dlopen/*.c
        SDL2/src/main/android/*.c
        SDL2/src/power/android/*.c
        SDL2/src/render/opengles/*.c
        SDL2/src/render/opengles2/*.c
        SDL2/src/render/software/*.c
        SDL2/src/thread/pthread/*.c
        SDL2/src/timer/unix/*.c
    )
    include_directories(
        ${SDL_INCLUDE_DIRS}
    )

    add_definitions(-DGL_GLEXT_PROTOTYPES)

    set(LIBRARY_DEPS
        GLESv1_CM
        GLESv2
        log
        android
        dl
    )
endif(ATOM_PLATFORM_ANDROID)
file(GLOB SDL_SOURCES
    SDL2/src/*.c
    SDL2/src/atomic/*.c
    SDL2/src/audio/*.c
    SDL2/src/core/*.c
    SDL2/src/cpuinfo/*.c
    SDL2/src/dynapi/*.c
    SDL2/src/events/*.c
    SDL2/src/file/*.c
    SDL2/src/filesystem/*.c
    SDL2/src/haptic/*.c
    SDL2/src/joystick/*.c
    SDL2/src/libm/*.c
    SDL2/src/loadso/*.c
   #SDL2/src/main/*.c
    SDL2/src/power/*.c
    SDL2/src/render/*.c
    SDL2/src/stdlib/*.c
    SDL2/src/thread/*.c
    SDL2/src/timer/*.c
    SDL2/src/video/*.c
)

if(NOT ATOM_PLATFORM_X11)

add_library(SDL2
        ${SDL_SOURCES}
        ${SDL_SOURCES_PLATFORM}
)

if(ATOM_PLATFORM_IOS)

    set_target_properties(SDL2 PROPERTIES
        COMPILE_FLAGS ${SDL_COMPILE_FLAGS}
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
        XCODE_ATTRIBUTE_COMPRESS_PNG_FILES "YES"
        XCODE_ATTRIBUTE_VALIDATE_PRODUCT "YES"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0"
    )

endif(ATOM_PLATFORM_IOS)
target_link_libraries(SDL2 ${LIBRARY_DEPS})
install (TARGETS SDL2 DESTINATION .)

endif(NOT ATOM_PLATFORM_X11)

#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_OLD})


