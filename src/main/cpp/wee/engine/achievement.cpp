#include <engine/achievement.hpp>

using nlohmann::json;

namespace wee {
	void to_json(json& j, const condition& c) {
        j = {
            { "key", c._key },
            { "activation_value", c._activationValue },
            { "condition", achievement::get_compare_string(c._cond) }
        };
            
    }

	void from_json(const json& j, condition& c) {
        c._key = j.at("key").get<std::string>();
        c._activationValue = j.at("activation_value").get<std::string>();
        c._cond = achievement::get_compare_func(j.at("condition").get<std::string>());
    }
	void to_json(json& j, const achievement& a) {
        j = {
                { "name", a._name },
                { "text", a._text },
                { "unlocked", a._unlocked },
                { "conditions", a._cond }
        };

			/*cJSON* json = cJSON_CreateObject();
			cJSON* arr = nullptr;
			{
				cJSON_AddStringToObject(json, "name", _name.c_str());//cJSON_AddStringToObject(json, "key", _key);
				cJSON_AddStringToObject(json, "text", _text.c_str());
				cJSON_AddBoolToObject(json, "unlocked", _unlocked);
				cJSON_AddItemToObject(json, "conditions", arr = cJSON_CreateArray());

				for(auto& c : _cond) 
					cJSON_AddItemToArray(arr, c.serialize());
			}
			return json;*/
    }
	void from_json(const json& j, achievement& a) {
        a._name     = j.at("name").get<std::string>();
        a._text     = j.at("text").get<std::string>();
        a._unlocked = j.at("unlocked").get<bool>();
        a._cond     = j.at("conditions").get<std::vector<condition> >();

        
        /*
         * 
			cJSON* ptr = json->child;
			while(ptr) {
				if(!strcmp(ptr->string, "name")) _name=ptr->valuestring;
				if(!strcmp(ptr->string, "text")) _text=ptr->valuestring;
				if(!strcmp(ptr->string, "unlocked")) _unlocked = ptr->valueint;
				if(!strcmp(ptr->string, "conditions")) {
					int n = cJSON_GetArraySize(ptr);
					for(int i=0; i < n; i++) {
						condition c;
						c.deserialize(cJSON_GetArrayItem(ptr, i));
						_cond.push_back(c);
					}
				}


				ptr = ptr->next;
         */
    }
	void to_json(json&, const achieve&) {
    }
	void from_json(const json&, achieve&) {
    }
}


