PROJECT(wee_engine)

FILE(GLOB SOURCES
    "*.cpp"
    "*.c"
)

ADD_LIBRARY(
    ${PROJECT_NAME}
    ${LIBRARY_TYPE}
    ${SOURCES}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} wee_base wee_core SDL2 SDL_ttf liquidfun assimp)


INSTALL(TARGETS ${PROJECT_NAME} DESTINATION .)
