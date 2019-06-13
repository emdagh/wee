
file( GLOB COMMON_SOURCES common/*.cpp)
file( GLOB APP_SOURCES *.cpp )

find_package(OpenGL)

IF(UNIX)
SET(PLATFORM_LIBS
	pthread
	dl
	sndio
)
ELSEIF(WIN32)
SET(PLATFORM_LIBS)
ENDIF()

foreach( testsourcefile ${APP_SOURCES} )
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    message(${testname})

    get_filename_component(barename ${testname} NAME)
    message(STATUS ${barename})

    add_executable( ${barename} ${testsourcefile} ${COMMON_SOURCES} )
    target_include_directories(
        ${barename} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(
        ${barename}
        wee_engine
        wee_base
        wee_gfx
        wee_core
        tmxlite
        liquidfun
        ${OPENGL_gl_LIBRARY}
        SDL_ttf
        SDL2_image
        SDL2
        ${PLATFORM_LIBS}
        assimp

    )
endforeach( testsourcefile ${APP_SOURCES} )
