include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/wee
    ${CMAKE_SOURCE_DIR}/vendor/date/include
)


file( GLOB APP_SOURCES *.cpp )
foreach( testsourcefile ${APP_SOURCES} )
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    message(${testname})

    get_filename_component(barename ${testname} NAME)
    message(STATUS ${barename})

    add_executable( ${barename} ${testsourcefile} )
    target_link_libraries(
        ${barename}
        wee_core
        wee_base
        wee_base_SDL2
        wee_gfx

        pthread
        dl
        sndio

    )
endforeach( testsourcefile ${APP_SOURCES} )
