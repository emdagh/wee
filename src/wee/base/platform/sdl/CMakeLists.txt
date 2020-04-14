project(wee_base_sdl)

file(GLOB SRC
    "*.cpp"
    "*.c"
)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_link_libraries(${PROJECT_NAME})
