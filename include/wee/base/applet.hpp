#pragma once

namespace wee {
    class applet {
        virtual int create() = 0;
        virtual int destroy() = 0;
        virtual int update(int, int) = 0;
        virtual int draw() = 0;
    };
}
