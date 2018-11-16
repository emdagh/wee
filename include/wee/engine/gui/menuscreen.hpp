#pragma once

#include <engine/gui/gamescreen.hpp>

namespace wee {

    class menuscreen : public gamescreen {
        typedef std::function<void(void)> callback;
        struct menuitem {
            std::string name;
            std::string callback;
        };
        std::map<std::string, callback> _callbacks;
    public:
        virtual void from_json(const json&);
        virtual void set_callback(const std::string&, const std::string&, const std::string&, callback&&);
        
    };
}
