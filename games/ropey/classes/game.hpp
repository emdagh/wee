#include <base/applet.hpp>
#include <map>
#include <vector>

namespace wee {
    struct application;
    struct gamescreen;
}

class game : public wee::applet {
    std::vector<wee::gamescreen*> _screens;
    std::map<std::string, size_t> _screen_index;
public:
    game();
    virtual ~game();
    int load_content(); 
    int update(int dt) ;
    int draw(wee::graphics_device*);
    void set_callbacks(wee::application*);
};
