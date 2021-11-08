#pragma once

#if defined(__LINUX__)
#include <dlfcn.h>
#define PLATFORM_PREFIX "lib"
#define PLATFORM_SUFFIX ".so"
#elif defined(__WIN32__)
#define PLATFORM_PREFIX ""
#define PLATFORM_SUFFIX ".dll"
#endif

namespace wee
{
    struct shared_library
    {
        template <typename T>
        static T import(const char* filename, const char* symbol)
        {
            std::stringstream ss;
            ss << PLATFORM_PREFIX << filename << PLATFORM_SUFFIX;

            T* res = nullptr;
            if(void* hnd = dlopen(filename, RTLD_NOW); hnd != nullptr)
            {
                if(void* sym = dlsym(hnd, symbol); sym != nullptr)
                {
                    res = reinterpret_cast<T>(sym);
                } 
                else 
                {
                    throw std::runtime_error(dlerror());
                }
                dlclose(hnd);
            }
            else 
            {
                throw std::runtime_error(dlerror());
            }
            return res;
        }
    };
}
