#pragma once

#include <map>
#include <string>
#include <cstring>
#include <initializer_list>
#include <regex>
#include <nlohmann/json.hpp>
#include <core/lexical_cast.hpp>
#include <wee/wee.hpp>


namespace wee {

	enum class ECondition {
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};


	struct condition {
		std::string _key;
        std::string _activationValue;
		ECondition _cond;
	};

	struct WEE_EXPORT achievement {
		
		std::string _name;
		std::string _text;
		std::vector<condition> _cond;
		bool _unlocked;

        static std::string get_compare_string(const ECondition c) {
			if(c == ECondition::Always)      return "${always}"; 
			if(c == ECondition::Greater)     return ">"        ; 
			if(c == ECondition::GreaterEqual)return ">="       ; 
			if(c == ECondition::Equal)       return "=="       ; 
			if(c == ECondition::Less)        return "<"        ; 
			if(c == ECondition::LessEqual)   return "<="       ; 
			if(c == ECondition::NotEqual)    return "!="       ; 
			if(c == ECondition::Never)       return "${never}" ; 
            return "${never}";

        }

		static ECondition get_compare_func(const std::string& str) {
			if(str == "${always}")  return ECondition::Always;
			if(str == ">")          return ECondition::Greater;
			if(str == ">=")         return ECondition::GreaterEqual;
			if(str == "==")         return ECondition::Equal;
			if(str == "<")          return ECondition::Less;
			if(str == "<=")         return ECondition::LessEqual;
			if(str == "!=")         return ECondition::NotEqual;
			if(str == "${never}")   return ECondition::Never;
            return ECondition::Never;
		}

		template <typename T>
		bool compare(const std::string& s1, const std::string &s2, const ECondition comp) {
			
			T a = wee::lexical_cast<T>(s1);
			T b = wee::lexical_cast<T>(s2);
			switch(comp) {

				case ECondition::Always:        return true;
				case ECondition::Greater:       return a > b; 
				case ECondition::GreaterEqual:  return a >= b; 
				case ECondition::Equal:         return a == b;
				case ECondition::Less:          return a < b; 
				case ECondition::LessEqual:     return a <= b;
				case ECondition::NotEqual:      return a != b; 
				case ECondition::Never:         return false;
				default:                        return false;
			}
			
			return false;
		}

		
		achievement& with_condition(const std::string& s1, const ECondition comp, const std::string& s2) {
			condition item;
			item._key               = s1;
			item._activationValue   = s2;
			item._cond              = comp;
			_cond.push_back(item);
			return *this;
		}
	};


	struct achieve { 

		template <typename T>
		struct dictionary {
			typedef std::map<std::string, T> type;
		};

		dictionary<achievement*>::type _ach;
		dictionary<std::string>::type _props;

        std::function<void(const achievement&)> on_completed;

		void get_all(std::vector<achievement*>* list) {
			for(auto it = _ach.begin(); it != _ach.end(); it++) {
				achievement* a = (*it).second;
				list->push_back(a);
			}
		}

		void get_unlocked(std::vector<achievement*>* list) {
			for(auto it = _ach.begin(); it != _ach.end(); it++) {
				achievement* a = (*it).second;
				if(a->_unlocked)
					list->push_back(a);
			}
		}

		void check_achievements(std::vector<achievement*>* list) {
            /**
             * a regex is required to prevent lexigraphical compare on numeric values
             */
			static std::regex integer("(\\+|-)?[[:digit:]]+");
	//      static std::regex real("[-+]?[0-9]*\.?[0-9]+");

			list->clear();
		
			for(auto it = _ach.begin(); it != _ach.end(); ++it ) {

				achievement* a = (*it).second;
				if(!a->_unlocked) {
					size_t activeProps = 0;
		
					for(auto& c : a->_cond) {
						
						std::string& value = _props[c._key];

						if(std::regex_match(value, integer)) {
							if(std::regex_match(c._activationValue, integer)) {
								if(a->compare<int>(value, c._activationValue, c._cond)) {
									activeProps++;
								}
							} else {
								throw std::runtime_error("invalid types for operand");
							}
						} else {
							if(a->compare<std::string>(value, c._activationValue, c._cond)) {
								activeProps++;
							}
						}
					}
					if(activeProps == a->_cond.size()) {
						a->_unlocked = true;
                        on_completed(*a);
						list->push_back(a);
					}
				}
			}
		}
		
		achievement& def_achievement(const std::string& name) {
			achievement* res = new achievement;
			res->_name      = name;
			res->_unlocked  = false;
			_ach.insert(std::pair<std::string, achievement*>(name, res));
			return *res;
		}

		void def_property(const std::string& key, const std::string& val) {
			_props[key] = val;
		}
	};
}
