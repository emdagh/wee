cmake_minimum_required(VERSION 3.0)

project(liquidfun C CXX)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/liquidfun/liquidfun/Box2D
)

set(BOX2D_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/liquidfun/liquidfun/Box2D/Box2D)

file(GLOB liquidfun_SRC
    ${BOX2D_ROOT}/Collision/*.cpp
    ${BOX2D_ROOT}/Collision/Shapes/*.cpp
    ${BOX2D_ROOT}/Common/*.cpp
    ${BOX2D_ROOT}/Dynamics/*.cpp
    ${BOX2D_ROOT}/Dynamics/Joints/*.cpp
    ${BOX2D_ROOT}/Dynamics/Contacts/*.cpp
    ${BOX2D_ROOT}/Particle/*.cpp
)



add_library(liquidfun ${liquidfun_SRC})

if(ATOM_PLATFORM_IOS)
set_target_properties(liquidfun PROPERTIES
#COMPILE_FLAGS ${SDL_COMPILE_FLAGS}
XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
XCODE_ATTRIBUTE_COMPRESS_PNG_FILES "YES"
XCODE_ATTRIBUTE_VALIDATE_PRODUCT "YES"
XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0"
)

endif(ATOM_PLATFORM_IOS)

target_link_libraries(liquidfun)
