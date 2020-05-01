#pragma once

#include <wee/wee.h>

namespace wee {
    struct graphics_context {
        using display_t = void*;
        using surface_t = void*;

        virtual void create(display_t, surface_t) = 0;
        virtual bool make_current() = 0;
        virtual void swap() = 0;
    };
}

#if defined(__cplusplus)
using wee_graphics_context = wee::graphics_context;
#else
struct wee_graphics_context;
#endif

C_API int graphics_context_create(wee_graphics_context**, void*, void*);
C_API int graphics_context_destroy(wee_graphics_context*);
