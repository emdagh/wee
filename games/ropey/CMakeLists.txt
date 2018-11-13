PROJECT(ropey)

#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME} )

INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

FILE(GLOB SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/classes/*.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
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

    )
