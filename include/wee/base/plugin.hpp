#pragma once

#include <ngin/platform.hpp>
#include <base/dynamic_library.hpp>

#define WEE_API_VERSION_BASE 1000
#define WEE_PLUGIN(T, NAME, VER) \
extern "C" {\
    static ngin::basic_plugin* __init_plugin__() { \
        static T s; \
        return &s;\
    } \
    ngin::plugin_details exports = { \
        WEE_API_VERSION_BASE, \
        __FILE__, \
        #T, \
        NAME, \
        VER, \
        __init_plugin__ \
    }; \
}

namespace ngin
{
    struct MODULE_PUBLIC basic_plugin 
    {
        virtual ~basic_plugin() = default;
    };

    typedef basic_plugin*(*pfnInitializePlugin)(void);

    struct plugin_details
    {
        int api;
        const char* filename;
        const char* classname;
        const char* plugin_name;
        const char* plugin_version;
        pfnInitializePlugin init;
    };
}
