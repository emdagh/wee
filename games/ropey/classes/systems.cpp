#include <classes/systems.hpp>

using namespace wee;

void disable_and_hide(const entity_type& self) {
    for(const auto& child : kult::join<nested>()) {
        if(self == kult::get<nested>(child).parent) {
            disable_and_hide(child);
        }
    }
    /**
     * disable
     */
    if(kult::has<physics>(self)) {
        kult::get<physics>(self).body->SetActive(false);
    }
    /**
     * and hide
     */
    if(kult::has<visual>(self)) {
        kult::get<visual>(self).visible = false;
    }
    if(kult::has<pickup>(self)) {
        kult::get<pickup>(self).active = false;
    }
}
void enable_and_show(const entity_type& self) {
    for(const auto& child : kult::join<nested>()) {
        if(self == kult::get<nested>(child).parent) {
            enable_and_show(child);
        }
    }
    /**
     * enable..
     */
    if(kult::has<physics>(self)) {
        kult::get<physics>(self).body->SetActive(true);
    }
    /**
     * and show
     */
    if(kult::has<visual>(self)) {
        kult::get<visual>(self).visible = true;
    }

    if(kult::has<pickup>(self)) {
        kult::get<pickup>(self).active = true;
    }
}
