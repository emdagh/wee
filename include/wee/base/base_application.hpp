#pragma once

#include <base/application.hpp>

namespace wee {

    class base_application 
    : public application 
    {
    public:
        virtual ~base_application() {}
        virtual int create();
        virtual int start();
        virtual int resume();
        virtual int pause();
        virtual int stop();
        virtual int destroy();
    public:
        //event_handler<int(const std::string&)> on_text_input;
    };
}
