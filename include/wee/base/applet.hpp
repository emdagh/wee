#pragma once

struct SDL_Renderer;

namespace wee {
    class graphics_device;
    class applet {
    public:
        virtual ~applet(){} 
        virtual int load_content() = 0;
        //virtual void unload_content() = 0;
        virtual int update(int) = 0;
        virtual int draw(graphics_device*) = 0;
    };
}
