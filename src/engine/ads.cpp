#include <cstdint>
#include <engine/ads.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>
#include <core/logstream.hpp>

using nlohmann::json;

namespace wee {
    std::map<std::string, ad*> _ads;
/*

    void AdManager_buildAd(cJSON* json, ad* ad) {
        cJSON* ptr = json;
        while(ptr) {
//            if(!strcmp(ptr->string, "location")) {
//                ad->setLocation(ptr->valuestring);
//            }
            if(!strcmp(ptr->string, "pub-id")) {
                ad->setPublisherId(ptr->valuestring);
            }
            if(!strcmp(ptr->string, "app-id")) {
                ad->setAppId(ptr->valuestring);
//                ad->request();
                //ad->onAdIsDismissed.add(atom::delegate<void(Ad*)>(&AdManager_cacheNext));
            }
            ptr = ptr->next;
        }
    }

    void AdManager_Parse(cJSON* json) {
        static std::string name;
        if(json->string) {
            if(!json->valuestring) {
                name = json->string;
            }
            if(!strcmp(json->string, "type")) {
                std::cout << "creating ad: " << name << std::endl;
                ad* ad = ad_factory::instance().create(json->valuestring);
                AdManager_buildAd(json, ad);
                //_ads.push_back(ad);
                //ad->request();
                _ads[name] = ad;
            }

        }

        if(json->child) {
            AdManager_Parse(json->child);
        }

        if(json->next) {
            AdManager_Parse(json->next);
        }
    }

    */

    void ad_manager::cache(std::istream &is) {
        std::istreambuf_iterator<char> eos;
        std::string rw(std::istreambuf_iterator<char>(is), {});

        try {

            json j = json::parse(rw);
            for(auto& jc : j) {
                DEBUG_VALUE_OF(j);
                ad* obj = factory<ad*>::instance().create(jc["type"]);
                if(nullptr != obj) {
                    obj->set_app_id(jc["app-id"]);
                    obj->set_publisher_id(jc["pub-id"]);
                }
            }
        } 
        catch (json::parse_error& e) {
            DEBUG_LOG(e.what()); // e.id(), e.byte()
        }
    }
    
    ad* ad_manager::at(const std::string& name) {
        std::map<std::string, ad*>::iterator it = _ads.find(name);
        if(it != _ads.end()) {
            return (*it).second;
        }
        return NULL;
    }
}
