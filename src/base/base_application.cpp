#include <util/logstream.hpp>
#include <base/base_application.hpp>

using namespace wee;


int base_application::create() {
    DEBUG_METHOD();
    return this->on_create({this});
}
int base_application::start() {
    DEBUG_METHOD();
    return this->on_start({this});
}
int base_application::resume() {
    DEBUG_METHOD();
    return this->on_resume({this});
}
int base_application::pause() {
    DEBUG_METHOD();
    return this->on_pause({this});
}
int base_application::stop() {
    DEBUG_METHOD();
    return this->on_stop({this});
}
int base_application::destroy() {
    DEBUG_METHOD();
    return this->on_destroy({this});
}
