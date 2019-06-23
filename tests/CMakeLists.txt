
file( GLOB COMMON_SOURCES common/*.cpp)
file( GLOB APP_SOURCES *.cpp )

find_package(OpenGL)

IF(UNIX)
SET(PLATFORM_LIBS
	pthread
	dl
	sndio
    #X11
    #GLU X11 Xxf86vm Xrandr pthread Xi
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
        SDL2_ttf
        SDL2_image
        SDL2
		assimp
		# ${OPENGL_LIBRARIES}
		${PLATFORM_LIBS}


    )
endforeach( testsourcefile ${APP_SOURCES} )
