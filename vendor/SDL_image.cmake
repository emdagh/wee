CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(SDL2_image C)

add_definitions(-DLOAD_PNG)

if(ATOM_PLATFORM_ANDROID)
include_directories(
    SDL2_image/external/libpng-1.6.2
)
endif(ATOM_PLATFORM_ANDROID)

file(GLOB SDL_IMAGE_SOURCES
    SDL2_image/IMG.c
    SDL2_image/IMG_bmp.c
    SDL2_image/IMG_gif.c
    SDL2_image/IMG_ImageIO.c
    SDL2_image/IMG_jpg.c
    SDL2_image/IMG_lbm.c
    SDL2_image/IMG_pcx.c
    SDL2_image/IMG_png.c
    SDL2_image/IMG_pnm.c
    SDL2_image/IMG_tga.c
    SDL2_image/IMG_tif.c
    SDL2_image/IMG_xcf.c
    SDL2_image/IMG_xpm.c
    SDL2_image/IMG_xv.c
    SDL2_image/IMG_xxx.c
    SDL2_image/IMG_webp.c


)

FILE(GLOB SRC_LIBPNG 
   SDL2_image/external/libpng-1.6.2/png.c 
   SDL2_image/external/libpng-1.6.2/pngerror.c
   SDL2_image/external/libpng-1.6.2/pngget.c 
   SDL2_image/external/libpng-1.6.2/pngmem.c 
   SDL2_image/external/libpng-1.6.2/pngpread.c 
   SDL2_image/external/libpng-1.6.2/pngread.c 
   SDL2_image/external/libpng-1.6.2/pngrio.c 
   SDL2_image/external/libpng-1.6.2/pngrtran.c 
   SDL2_image/external/libpng-1.6.2/pngrutil.c
   SDL2_image/external/libpng-1.6.2/pngset.c 
   SDL2_image/external/libpng-1.6.2/pngtrans.c 
   SDL2_image/external/libpng-1.6.2/pngwio.c 
   SDL2_image/external/libpng-1.6.2/pngwrite.c 
   SDL2_image/external/libpng-1.6.2/pngwtran.c 
   SDL2_image/external/libpng-1.6.2/pngwutil.c
)

#if(ATOM_PLATFORM_ANDROID)
file(GLOB SRC_LIBZ
   SDL2_image/external/zlib-1.2.8/*.c
)
#endif(ATOM_PLATFORM_ANDROID)


if(ATOM_PLATFORM_IOS)
file(GLOB SDL_IMAGE_SOURCES_PLATFORM
    SDL2_image/IMG_UIImage.m
)
endif(ATOM_PLATFORM_IOS)

add_library(z ${SRC_LIBZ})
#add_library(png ${SRC_LIBPNG})

add_library(SDL2_image ${SDL_IMAGE_SOURCES} ${SDL_IMAGE_SOURCES_PLATFORM})

if(ATOM_PLATFORM_IOS)
    set(SDL_IMAGE_COMPILE_FLAGS "-fno-objc-arc")
    set_target_properties(SDL_image PROPERTIES
        COMPILE_FLAGS ${SDL_IMAGE_COMPILE_FLAGS}
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
        XCODE_ATTRIBUTE_COMPRESS_PNG_FILES "YES"
        XCODE_ATTRIBUTE_VALIDATE_PRODUCT "YES"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0"
    )
endif(ATOM_PLATFORM_IOS)
target_link_libraries(SDL2_image png z)
install(TARGETS SDL2_image DESTINATION .)





