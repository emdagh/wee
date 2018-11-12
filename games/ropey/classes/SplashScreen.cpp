#include <engine/gui/gamescreen.hpp>

using namespace wee;

class splash_screen : public gamescreen {
    std::vector<SDL_Texture*> _images;
public:

    virtual ~splash_screen() {
    }
};


class mainmenu_screen : public gamescreen {
    std::vector<SDL_Texture*> _images;
public:

    virtual ~splash_screen() {
    }
};

class gameplay_screen : public gamescreen {
    std::vector<SDL_Texture*> _images;
public:

    virtual ~splash_screen() {
    }
};


class pause_screen : public gamescreen {
    std::vector<SDL_Texture*> _images;
public:

    virtual ~splash_screen() {
    }
};

