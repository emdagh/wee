
file( GLOB APP_SOURCES *.cpp )

find_package(OpenGL)

IF(UNIX)
SET(PLATFORM_LIBS
	pthread
	dl
	sndio
    X11
    Xi
)
ELSEIF(WIN32)
SET(PLATFORM_LIBS)
ENDIF()

foreach( sourcefile ${APP_SOURCES} )
    string( REPLACE ".cpp" "" name ${sourcefile} )
    message(${name})

    get_filename_component(barename ${name} NAME)
    message(STATUS ${barename})

    add_executable( ${barename} ${sourcefile} ${COMMON_SOURCES} )
    target_link_libraries(
        ${barename}
        wee_engine
        wee_base
        wee_gfx
        wee_core
        tmxlite
        liquidfun
        ${OPENGL_LIBRARIES}
        SDL2_ttf
        SDL2_image
        SDL2
        ${PLATFORM_LIBS}
        assimp
        glew

    )
endforeach( sourcefile ${APP_SOURCES} )
