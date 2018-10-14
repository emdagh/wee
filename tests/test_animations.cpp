#include <nlohmann/json.hpp>
#include <iostream>
#include <engine/assets.hpp>
#include <prettyprint.hpp>
#include "common/components.hpp"
#include <engine/applet.hpp>
#include <engine/application.hpp>
#include <fstream>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>

using wee::get_resource_path;
using nlohmann::json;

void to_json(json& j, const SDL_Rect& r) {
    j = json {
        {"x", r.x},
        {"y", r.y},
        {"w", r.w},
        {"h", r.h}
    };
}

std::ostream& operator << (std::ostream& os, const SDL_Rect& a) {
    json j;
    to_json(j, a); 
    return os << j;
}

#include <engine/sprite_sheet.hpp>
namespace wee {
    struct animation_info {
        sprite_sheet _sheet;
        std::map<std::string, std::vector<size_t> > _animations;
    };
    void to_json(json& j, const animation_info& a) {

    }

    void from_json(const json& j, animation_info& a) {

        a._sheet = j;
        const auto& frames = j["animations"];
        for(auto it = frames.begin(); it != frames.end(); it++) {
            for(const auto& j : it.value()) {
                a._animations[it.key()].push_back(a._sheet.index_of(j));
            }
        }
    }
    template <>
    struct assets<animation_info> : singleton<assets<animation_info> >{
        std::map<std::string, std::unique_ptr<animation_info> > _index;

        animation_info* get(const std::string& name) {
            if(_index.count(name) != 0) {
                return _index.at(name).get();
            }
            return nullptr;
        }

        animation_info* load(const std::string& name, std::istream& is) {
            if(_index.count(name) == 0) {
                animation_info* res = new animation_info;

                json j;
                j << is;
                from_json(j, *res);
                _index[name] = std::unique_ptr<animation_info>(res);

            }

            return _index.at(name).get();
        }
    };
}

using namespace wee;




typedef struct {
    std::string animation_name;
    std::string clip_name;
    //int state;
    //size_t frame;
    //size_t begin, end; // indices into an array with the actual indices of the rects
    bool loop;
    int fps;
} animation_t;

using animation = kult::component<1 << 10, animation_t>;
std::ostream& operator << (std::ostream& os, const animation_t&) {
    return os;
}

auto animations = [&] (int dt) {
    for(const auto& self : kult::join<timeout, animation, visual>()) {
        timeout_t& to   = kult::get<timeout>(self);
        animation_t& an = kult::get<animation>(self);
        visual_t& vi    = kult::get<visual>(self);


        if(to.time >= to.timeout) {
            if(an.loop) {
                to.time = 0;
            } else{
                to.time = to.timeout;
            }
            /**
             * perhaps an event handler call here? Tie it into an fsm perhaps...
             */
        }

        float lerp = static_cast<float>(to.time) / to.timeout;
        //an.frame = an.begin + (an.end - an.begin) * lerp; 
        auto* anim = assets<animation_info>::instance().get(an.animation_name);
        const auto& frames = anim->_animations.at(an.clip_name);
        to.timeout = static_cast<size_t>(1000.0f * frames.size() / an.fps);
        size_t ix = frames[static_cast<size_t>(lerp * (frames.size() - 1))];
        const auto& rc = anim->_sheet[ix];
        vi.src = rc;
        

        //animation_info::instance()[an.animation_name][an.frame]; 
        
    }
};

auto timeouts = [&] (int dt) {
    for(const auto& self : kult::join<timeout>() ) {
        timeout_t& to = kult::get<timeout>(self);
        to.time = std::min(to.time + dt, to.timeout);
        if(to.time >= to.timeout) {
            if(to.on_timeout != nullptr) 
                to.on_timeout(self);
        }
    }
};

struct game : wee::applet {
    int load_content() {
        assets<animation_info>::instance().load("@skeleton", ::as_lvalue(
                std::ifstream(get_resource_path("") + "assets/img/skeleton.json")
            )
        );

        kult::type e = kult::entity();
        kult::add<animation>(e) = {
            "@skeleton",
            "idle",
            true,
            15
        };

        kult::add<visual>(e) = {
            assets<SDL_Texture>::instance().get("assets/img/skeleton.png"),
            { 0, 0, 0, 0 },
            { 255, 255, 255, 255 }
        };

        kult::add<timeout>(e) = {
            0,
            1000,
            [&] (const kult::type& self) {
                kult::get<animation>(self).clip_name = "attack";
            }
        };


        kult::add<transform>(e) = {
            { 0.f, 0.f },
            0.0f
        };
        return 0;
    }

    int update(int dt) {
        timeouts(dt);
        animations(dt);

        return 0;
    }

    int draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
        for(const auto& e : kult::join<transform, visual>()) {
            const visual_t& v = kult::get<visual>(e);
            const transform_t& t = kult::get<transform>(e);
            SDL_Rect dst = {
                t.p.x, 
                t.p.y,
                v.src.w, 
                v.src.h
            };
            SDL_RenderCopy(renderer, 
                v.texture,
                &v.src,
                &dst
            );
        }
        SDL_RenderPresent(renderer);
    }
};


int main(int argc, char* argv[]) {

    wee::applet *let = new game();
    wee::application app(let);
    return app.start();

#if 0
    json j;
    j << is;

    sprite_sheet s = j;

    std::map<std::string, std::vector<size_t> > _index;

    const auto& frames = j["animations"];
    for(auto it = frames.begin(); it != frames.end(); it++) {
        for(const auto& j : it.value()) {
            _index[it.key()].push_back(s.index_of(j));
        }
    }
    DEBUG_VALUE_OF(_index);
    auto frames = j["frames"];
    std::map<std::string, SDL_Rect> _frames;
    for(auto it = frames.begin(); it != frames.end(); it++) {
        const auto& jj = it.value()["frame"];
        SDL_Rect r = (SDL_Rect) {
            jj.at("x").get<int>(),
            jj.at("y").get<int>(),
            jj.at("w").get<int>(),
            jj.at("h").get<int>()
        };
        _frames[it.key()] = r;
    }
    DEBUG_VALUE_OF(_frames);
    auto animations = j["animations"];
    for(const auto& a: animations) {
        DEBUG_VALUE_AND_TYPE_OF(a);
    }
    auto image_path = j["meta"]["image"];
#endif
    return 0;
}
