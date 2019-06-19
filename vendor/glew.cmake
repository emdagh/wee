#add_subdirectory(glew)

cmake_minimum_required(VERSION 3.0)
project(glew C)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/include
)


set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/src/glew.c
)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_link_libraries(${PROJECT_NAME})

