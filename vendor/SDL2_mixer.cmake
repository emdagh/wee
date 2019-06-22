CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(SDL_mixer C)

set(DIR_MIXER       ${CMAKE_CURRENT_SOURCE_DIR}/SDL2_mixer)
set(DIR_EXTERNAL    ${DIR_MIXER}/external)
set(DIR_OGG         ${DIR_EXTERNAL}/libogg-1.3.2)
set(DIR_VORBIS      ${DIR_EXTERNAL}/libvorbis-1.3.5)
set(DIR_FLAC        ${DIR_EXTERNAL}/flac-1.3.2)
set(DIR_MP3         ${DIR_EXTERNAL}/smpeg2-2.0.0)

include_directories(
    ${DIR_MIXER}
    ${DIR_OGG}/include
    ${DIR_VORBIS}/include
    ${DIR_VORBIS}/lib
    ${DIR_FLAC}/include
    ${DIR_MP3}
)

file(GLOB SDL_mixer_src
    ${DIR_MIXER}/*.c
    #${DIR_OGG}/src/*.c
    # ${DIR_VORBIS}/lib/*.c
    # ${DIR_EXTERNAL}/external/libvorbisidec-1.2.1/*.c
    ${DIR_EXTERNAL}/libmodplug-0.8.8.4/*.c
    ${DIR_MIXER}/timidity/*.c
)

list(REMOVE_ITEM SDL_mixer_src "${DIR_VORBIS}/lib/psytune.c")
list(REMOVE_ITEM SDL_mixer_src "${DIR_VORBIS}/lib/tone.c")
list(REMOVE_ITEM SDL_mixer_src "${DIR_MIXER}/playwave.c")
list(REMOVE_ITEM SDL_mixer_src "${DIR_MIXER}/playmus.c")

add_definitions(
    -DMUSIC_WAV
    #-DMUSIC_OGG
    #-DMUSIC_MID
#    -DMP3_MUSIC
#    -DFLAC_MUSIC
)

add_library(SDL_mixer ${SDL_mixer_src})

if(ATOM_PLATFORM_IOS)
set_target_properties(SDL_mixer PROPERTIES
COMPILE_FLAGS ${SDL_COMPILE_FLAGS}
XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
XCODE_ATTRIBUTE_COMPRESS_PNG_FILES "YES"
XCODE_ATTRIBUTE_VALIDATE_PRODUCT "YES"
XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0"
)

endif(ATOM_PLATFORM_IOS)

target_link_libraries(SDL_mixer SDL2)

install(TARGETS SDL_mixer DESTINATION .)
