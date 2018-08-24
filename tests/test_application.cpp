#include <util/logstream.hpp>
#include <base/platform_application.hpp>
#include <iostream>

using namespace wee;

int main(int argc, char* argv[]) {
    DEBUG_METHOD();
    application* app = NULL;
    try {
        app = new platform_application;
    } catch(...) {
        std::cout << "exception caught" << std::endl;
    }
    return app->start();
}
