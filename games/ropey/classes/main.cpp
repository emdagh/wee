#include <base/application.hpp>
#include <classes/game.hpp>

//extern "C" new_game();
//extern "C" init_game(application*, applet*);

using namespace wee;

#undef main //SDL idiocy
int main(int, char*[]) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->set_callbacks(&app);
    return app.start();
}

