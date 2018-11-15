#include <classes/screens.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace {
    static class register_gamescreen_factories {
        typedef gamescreen_factory  factory_type; 
    public:
        register_gamescreen_factories() {  
            factory_type::instance().register_class("splash_screen", [] (void) {
                return new splash_screen();
            });
        }

        virtual ~register_gamescreen_factories() {
        }
    }_;
}
