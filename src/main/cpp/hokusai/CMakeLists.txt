project(hokusai)
file(GLOB SOURCES
    "*.cpp"
)
add_library(
    ${PROJECT_NAME}
    ${LIBARY_TYPE}
    ${SOURCES}
)
target_link_libraries(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} DESTINATION .)
