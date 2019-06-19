
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

SET(FT2_BASE SDL2_ttf/external/freetype-2.4.12)
FILE(GLOB SRC_FREETYPE
	${FT2_BASE}/src/autofit/autofit.c
	${FT2_BASE}/src/base/ftbase.c
	${FT2_BASE}/src/base/ftbbox.c
	${FT2_BASE}/src/base/ftbdf.c
	${FT2_BASE}/src/base/ftbitmap.c
	${FT2_BASE}/src/base/ftcid.c
	${FT2_BASE}/src/base/ftfntfmt.c
	${FT2_BASE}/src/base/ftfstype.c
	${FT2_BASE}/src/base/ftgasp.c
	${FT2_BASE}/src/base/ftglyph.c
	${FT2_BASE}/src/base/ftgxval.c
	${FT2_BASE}/src/base/ftinit.c
	${FT2_BASE}/src/base/ftlcdfil.c
	${FT2_BASE}/src/base/ftmm.c
	${FT2_BASE}/src/base/ftotval.c
	${FT2_BASE}/src/base/ftpatent.c
	${FT2_BASE}/src/base/ftpfr.c
	${FT2_BASE}/src/base/ftstroke.c
	${FT2_BASE}/src/base/ftsynth.c
	${FT2_BASE}/src/base/ftsystem.c
	${FT2_BASE}/src/base/fttype1.c
	${FT2_BASE}/src/base/ftwinfnt.c
	${FT2_BASE}/src/bdf/bdf.c
	${FT2_BASE}/src/bzip2/ftbzip2.c
	${FT2_BASE}/src/cache/ftcache.c
	${FT2_BASE}/src/cff/cff.c
	${FT2_BASE}/src/cid/type1cid.c
	${FT2_BASE}/src/gzip/ftgzip.c
	${FT2_BASE}/src/lzw/ftlzw.c
	${FT2_BASE}/src/pcf/pcf.c
	${FT2_BASE}/src/pfr/pfr.c
	${FT2_BASE}/src/psaux/psaux.c
	${FT2_BASE}/src/pshinter/pshinter.c
	${FT2_BASE}/src/psnames/psnames.c
	${FT2_BASE}/src/raster/raster.c
	${FT2_BASE}/src/sfnt/sfnt.c
	${FT2_BASE}/src/smooth/smooth.c
	${FT2_BASE}/src/truetype/truetype.c
	${FT2_BASE}/src/type1/type1.c
	${FT2_BASE}/src/type42/type42.c
	${FT2_BASE}/src/winfonts/winfnt.c
)

add_library(freetype2 STATIC ${SRC_FREETYPE})
add_library(${PROJECT_NAME} ${SDL_TTF_SOURCES})

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

target_link_libraries(${PROJECT_NAME} freetype2 SDL2)
