
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(wee VERSION 1.0.0 LANGUAGES C CXX)

message("CMAKE_INSTALL_DIR=${CMAKE_INSTALL_DIR}")

set(default_build_type "Debug")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

IF(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic -Wall -Wextra -std=c++17 -ggdb -O0") #-Wextra
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden)

    IF(ANDROID)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${ANDROID_ABI}/${CMAKE_BUILD_TYPE} )
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${ANDROID_ABI}/${CMAKE_BUILD_TYPE} )
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${ANDROID_ABI}/${CMAKE_BUILD_TYPE} )
    ELSEIF(IOS)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-abi-version=2 -fobjc-arc -x objective-c++")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
    ELSE()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2")
        SET(BUILD_SHARED_LIBS, TRUE)
    ENDIF()
ELSEIF(WIN32)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
    SET(BUILD_SHARED_LIBS FALSE)
    SET(BUILD_STATIC_LIBS TRUE)
    #SET(BUILD_SHARED_LIBS TRUE)
    #SET(BUILD_STATIC_LIBS FALSE)
ENDIF()

MESSAGE(STATUS "Output directory (libraries/archives): '${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}'")
MESSAGE(STATUS "Output directory (binaries): '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")

ADD_DEFINITIONS(-DBUILD_LIBRARY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/wee/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/wee/config.h
)

INCLUDE_DIRECTORIES(
    ${CMAKE_BINARY_DIR}/vendor/SDL2/include
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_BINARY_DIR}/include/wee
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/wee
    ${CMAKE_SOURCE_DIR}/vendor/liquidfun/liquidfun/Box2D
    ${CMAKE_SOURCE_DIR}/vendor/SDL2/include
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_image
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_ttf
    ${CMAKE_SOURCE_DIR}/vendor/SDL2_mixer
    ${CMAKE_SOURCE_DIR}/vendor/date/include
    ${CMAKE_SOURCE_DIR}/vendor/fmt/include
    ${CMAKE_SOURCE_DIR}/vendor/json/include
    ${CMAKE_SOURCE_DIR}/vendor/kult
    ${CMAKE_SOURCE_DIR}/vendor/cxx-prettyprint
    ${CMAKE_SOURCE_DIR}/vendor/tmxlite/tmxlite/include
    ${CMAKE_SOURCE_DIR}/vendor/assimp/include
    ${CMAKE_BINARY_DIR}/vendor/assimp/include
    ${CMAKE_SOURCE_DIR}/vendor/glew/include
)
LINK_DIRECTORIES(
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)


ADD_SUBDIRECTORY(vendor)
ADD_SUBDIRECTORY(src)

REMOVE_DEFINITIONS(-DBUILD_LIBRARY)

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

set(CMAKE_CXX_CLANG_TIDY
  clang-tidy;
  -checks=*;
  -warnings-as-errors=*;)

