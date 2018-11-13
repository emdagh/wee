#include <base/application.hpp>
#include <classes/game.hpp>

using namespace wee;

#undef main //SDL idiocy
int main(int, char*[]) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->callbacks(&app);
    return app.start();
}

