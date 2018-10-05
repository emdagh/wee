
file( GLOB COMMON_SOURCES common/*.cpp)
file( GLOB APP_SOURCES *.cpp )

foreach( testsourcefile ${APP_SOURCES} )
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    message(${testname})

    get_filename_component(barename ${testname} NAME)
    message(STATUS ${barename})

    add_executable( ${barename} ${testsourcefile} ${COMMON_SOURCES} )
    target_link_libraries(
        ${barename}
        wee_engine
        wee_base
        wee_gfx
        wee_core
        tmxlite
        liquidfun
        GL
        SDL_ttf
        SDL2_image
        SDL2
        pthread
        dl
        sndio

    )
endforeach( testsourcefile ${APP_SOURCES} )
