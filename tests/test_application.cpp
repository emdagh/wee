#include <base/platform_application.hpp>
#include <iostream>

using namespace wee;

int main(int argc, char* argv[]) {

    application* app = NULL;
    try {
        app = new platform_application;
    } catch(...) {
        std::cout << "exception caught" << std::endl;
    }
    return app->start();
}
