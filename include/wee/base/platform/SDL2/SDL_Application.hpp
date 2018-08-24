#pragma once

#include <base/base_application.hpp>

namespace wee {
    namespace detail {

        class SDL_Application : public base_application {
            struct impl;
            impl* _pimpl;

        public:
            SDL_Application();
            virtual ~SDL_Application();

            virtual int create();
            virtual int start();
            virtual int resume();
            virtual int pause();
            virtual int stop();
            virtual int destroy();
        };
    }
}
