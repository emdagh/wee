
cmake_minimum_required(VERSION 3.0)

project(SDL2_ttf C)

include_directories(
    SDL2_ttf/external/freetype-2.4.12/include
    SDL2_ttf/external/freetype-2.4.12/include/freetype

)
add_definitions(-DFT2_BUILD_LIBRARY)
file(GLOB SDL_TTF_SOURCES
    SDL2_ttf/SDL_ttf.c
    #SDL2_ttf/glfont.c
)

FILE(GLOB SRC_FREETYPE
    #SDL2_ttf/external/freetype-2.4.12/src/autofit/*.c
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afangles.c 
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afcjk.c    
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afdummy.c  
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afglobal.c 
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afhints.c  
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afindic.c  
    SDL2_ttf/external/freetype-2.4.12/src/autofit/aflatin.c  
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afloader.c 
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afmodule.c 
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afpic.c    
    SDL2_ttf/external/freetype-2.4.12/src/autofit/afwarp.c
    SDL2_ttf/external/freetype-2.4.12/src/base/ftadvanc.c 
    SDL2_ttf/external/freetype-2.4.12/src/base/ftbitmap.c
    SDL2_ttf/external/freetype-2.4.12/src/base/ftcalc.c   
    SDL2_ttf/external/freetype-2.4.12/src/base/ftdbgmem.c 
    SDL2_ttf/external/freetype-2.4.12/src/base/ftgloadr.c
    SDL2_ttf/external/freetype-2.4.12/src/base/ftglyph.c
    SDL2_ttf/external/freetype-2.4.12/src/base/ftinit.c 
    SDL2_ttf/external/freetype-2.4.12/src/base/ftobjs.c   
    SDL2_ttf/external/freetype-2.4.12/src/base/ftoutln.c  
    SDL2_ttf/external/freetype-2.4.12/src/base/ftrfork.c  
    SDL2_ttf/external/freetype-2.4.12/src/base/ftsnames.c 
    SDL2_ttf/external/freetype-2.4.12/src/base/ftstroke.c
    SDL2_ttf/external/freetype-2.4.12/src/base/ftstream.c 
    SDL2_ttf/external/freetype-2.4.12/src/base/ftsystem.c
    SDL2_ttf/external/freetype-2.4.12/src/base/fttrigon.c 
    SDL2_ttf/external/freetype-2.4.12/src/base/ftutil.c
    #SDL2_ttf/external/freetype-2.4.12/src/base/*.c
    SDL2_ttf/external/freetype-2.4.12/src/bdf/*.c
    SDL2_ttf/external/freetype-2.4.12/src/bzip2/*.c
    SDL2_ttf/external/freetype-2.4.12/src/cache/*.c
    SDL2_ttf/external/freetype-2.4.12/src/cff/*.c
    SDL2_ttf/external/freetype-2.4.12/src/cid/*.c
    #SDL2_ttf/external/freetype-2.4.12/src/gxvalid/*.c
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvcommn.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvfeat.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvbsln.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvtrak.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvopbd.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvprop.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvjust.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmort.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmort0.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmort1.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmort2.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmort4.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmort5.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmorx.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmorx0.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmorx1.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmorx2.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmorx4.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmorx5.c 
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvlcar.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvkern.c  
    SDL2_ttf/external/freetype-2.4.12/src/gxvalid/gxvmod.c
    SDL2_ttf/external/freetype-2.4.12/src/gzip/ftgzip.c
    SDL2_ttf/external/freetype-2.4.12/src/lzw/*.c
    SDL2_ttf/external/freetype-2.4.12/src/otvalid/*.c
    SDL2_ttf/external/freetype-2.4.12/src/pcf/*.c
    SDL2_ttf/external/freetype-2.4.12/src/pfr/*.c
    SDL2_ttf/external/freetype-2.4.12/src/psaux/*.c
    SDL2_ttf/external/freetype-2.4.12/src/pshinter/*.c
    SDL2_ttf/external/freetype-2.4.12/src/psnames/*.c
    SDL2_ttf/external/freetype-2.4.12/src/raster/*.c
    SDL2_ttf/external/freetype-2.4.12/src/sfnt/*.c
    SDL2_ttf/external/freetype-2.4.12/src/smooth/*.c
    #SDL2_ttf/external/freetype-2.4.12/src/tools/*.c
    SDL2_ttf/external/freetype-2.4.12/src/truetype/*.c
    SDL2_ttf/external/freetype-2.4.12/src/type1/*.c
    SDL2_ttf/external/freetype-2.4.12/src/type42/*.c
    SDL2_ttf/external/freetype-2.4.12/src/winfonts/*.c
)

add_library(freetype2 ${SRC_FREETYPE})
add_library(SDL2_ttf ${SDL_TTF_SOURCES})

if(IOS)
    set_target_properties(SDL2_ttf PROPERTIES
        COMPILE_FLAGS ${SDL_COMPILE_FLAGS}
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
        XCODE_ATTRIBUTE_COMPRESS_PNG_FILES "YES"
        XCODE_ATTRIBUTE_VALIDATE_PRODUCT "YES"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0"
    )

#set_xcode_property (SDL2 IPHONEOS_DEPLOYMENT_TARGET "7.1")
endif()

target_link_libraries(SDL2_ttf freetype2)
