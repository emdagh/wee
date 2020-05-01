#include <base/application.hpp>

int main(int, char**) {
    wee::applicatoin* app = nullptr;
    wee::shared_library* lib = new wee::shared_library();
    lib->open("wee_base_sdl");
    if(lib->is_open()) {
        auto pfn = lib->sym<pfnApplicationCreate>("application_create");
        pfn(&app);
    }
    return 0;
}
