CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(SDL2_image C)

add_definitions(-DLOAD_PNG)

set(LIBPNG_VERSION "1.6.37")
set(LIBZ_VERSION "1.2.11")

SET(PNG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image/external/libpng-${LIBPNG_VERSION})
SET(Z_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/SDL2_image/external/zlib-${LIBZ_VERSION})

message(${PNG_ROOT})
message(${Z_ROOT})


#if(ATOM_PLATFORM_ANDROID)
include_directories(
	${PNG_ROOT}
	${Z_ROOT}

	#SDL2_image/external/libpng-1.6.32
	#SDL2_image/external/zlib-1.2.11
)
#endif(ATOM_PLATFORM_ANDROID)

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
    SDL2_image/IMG_svg.c
    SDL2_image/IMG_tga.c
    SDL2_image/IMG_tif.c
    SDL2_image/IMG_xcf.c
    SDL2_image/IMG_xpm.c
    SDL2_image/IMG_xv.c
    SDL2_image/IMG_xxx.c
    SDL2_image/IMG_webp.c


)

FILE(GLOB SRC_LIBPNG 
   ${PNG_ROOT}/png.c 
   ${PNG_ROOT}/pngerror.c
   ${PNG_ROOT}/pngget.c 
   ${PNG_ROOT}/pngmem.c 
   ${PNG_ROOT}/pngpread.c 
   ${PNG_ROOT}/pngread.c 
   ${PNG_ROOT}/pngrio.c 
   ${PNG_ROOT}/pngrtran.c 
   ${PNG_ROOT}/pngrutil.c
   ${PNG_ROOT}/pngset.c 
   ${PNG_ROOT}/pngtrans.c 
   ${PNG_ROOT}/pngwio.c 
   ${PNG_ROOT}/pngwrite.c 
   ${PNG_ROOT}/pngwtran.c 
   ${PNG_ROOT}/pngwutil.c
)

#if(ATOM_PLATFORM_ANDROID)
file(GLOB SRC_LIBZ
	${Z_ROOT}/*.c
)
#endif(ATOM_PLATFORM_ANDROID)


if(ATOM_PLATFORM_IOS)
file(GLOB SDL_IMAGE_SOURCES_PLATFORM
    SDL2_image/IMG_UIImage.m
)
endif(ATOM_PLATFORM_IOS)

add_library(z_${LIBZ_VERSION} STATIC  ${SRC_LIBZ})
add_library(png_${LIBPNG_VERSION} STATIC ${SRC_LIBPNG})

add_library(SDL2_image SHARED ${SDL_IMAGE_SOURCES} ${SDL_IMAGE_SOURCES_PLATFORM})

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
target_link_libraries(png_${LIBPNG_VERSION} z_${LIBZ_VERSION})
target_link_libraries(SDL2_image SDL2 png_${LIBPNG_VERSION} z_${LIBZ_VERSION})
install(TARGETS SDL2_image DESTINATION .)





