PROJECT(wee_gfx)
FILE(GLOB SOURCES
    "*.cpp"
    "*.c"
)
find_package(OpenGL REQUIRED)
ADD_LIBRARY(
    ${PROJECT_NAME}
    ${SOURCES}
)
IF(WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME}) # wee_core SDL2 glew ${OPENGL_LIBRARIES})
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION .)