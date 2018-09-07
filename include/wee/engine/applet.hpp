#pragma once

struct SDL_Renderer;

namespace wee {
    class applet {
    public:
        virtual ~applet(){} 
        virtual int load_content() = 0;
        virtual int update(int) = 0;
        virtual int draw(SDL_Renderer*) = 0;
    };
}
