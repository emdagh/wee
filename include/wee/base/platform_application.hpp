#pragma once

#include <wee/wee.hpp>

#if (WEE_PLATFORM == WEE_PLATFORM_UNDEFINED)
#include <base/platform/SDL2/SDL_Application.hpp>
namespace wee {
    typedef wee::detail::SDL_Application platform_application;
}
#endif
