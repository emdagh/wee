#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>


std::vector<std::string> string_split_on_all(const std::string& s, char c) {
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string token;
    while(std::getline(ss, token, c)) {
        res.emplace_back(token);
    }
    return res;
}

std::pair<std::string, std::string> string_split_on_first(const std::string& s, char c) {
    auto pos = s.find_first_of(static_cast<const char*>(&c));
    return { s.substr(0, pos), s.substr(pos + 1, s.npos) };
}

std::string ltrim(const std::string& str, const std::string& what) {
    std::string copy(str);
    copy.erase(0, copy.find_first_not_of(what));
    return copy;
}

std::string string_normalize(const std::string& s, char c) {
    std::string copy(s);
    auto it = std::unique(copy.begin(), copy.end(), [&] (auto a, auto b) {
      return a == b && a == c;
    });
    copy.erase(it, copy.end());
    return copy;
}

template <typename T>
std::string lexical_cast(const T& t) {
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

template <typename T>
T lexical_cast(const std::string& s) {
    T res;
    std::istringstream ss;
    ss.str(s);
    ss >> res;
    return res;
}


template <typename T>
class key_not_found : public std::runtime_error {
public:
    explicit key_not_found(const T& key) : std::runtime_error(lexical_cast<T>(key)) {}
    virtual ~key_not_found() = default;
};

template <typename K, typename V>
class basic_person {
public:
    typedef K key_type;
    typedef V value_type;
    typedef std::unordered_map<key_type, value_type> traits_type;

    constexpr static const char* kJohnDoe = "Unknown";
private:

    traits_type _traits;
protected:
    inline const value_type& _try_get_trait(const key_type& k) const {
        if(_traits.count(k) == 0) {
            throw key_not_found<key_type>(k);
        }
        return _traits.at(k);
    }
public:
    basic_person() {

    }
    virtual ~basic_person() {

    }
    const value_type& get_trait(const key_type& k) const {
        return _try_get_trait(k);
    }
    void set_trait(const key_type& k, const value_type& v) {
        _traits[k] = v;
    }

    const traits_type& get_traits() const {
        return _traits;
    }

    bool has_trait(const key_type& key) const {
        return _traits.count(key) > 0;
    }

    bool has_all_traits(const traits_type& t) const {
        for(auto it: t) {
            if(!has_trait(it.first)) 
                continue;
            if(get_trait(it.first) != it.second) 
                return false;
        }
        return true;
    }


    bool has_trait_value(const value_type& val) const {
        return std::find_if(_traits.begin(), _traits.end(), [&] (const auto& t) {
            return t.second == val;
        }) != _traits.end();
    }
    static basic_person from_string(const std::string& s) {
        basic_person t;
        auto all = string_split_on_all(string_normalize(s, ' '), ' ');
        for(const auto& i : all) {
            auto pair = string_split_on_all(i, '=');
            if(pair.size() != 2) throw std::runtime_error("malformed string in input");

            t.set_trait(pair[0], pair[1]);//.first, pair.second);
        }
        return t;
    }

    static const basic_person& unknown_person() {
        static basic_person p = basic_person::from_string("Name=Unknown");
        return p;
    }
};

typedef basic_person<std::string, std::string> person;


template <typename K, typename V>
class basic_registry {
    typedef K key_type;
    typedef V value_type;
    std::vector<basic_person<K, V> > _people;
    typedef typename basic_person<K, V>::traits_type traits_type;

public:
    const person* find(const person::traits_type& t) {
        for(const auto& p: _people) {
           if(p.has_all_traits(t))
               return &p;
        }
        return nullptr;
    }

    void add(const person&& p) {
        _people.emplace_back(std::move(p));
    }
};

typedef basic_registry<std::string, std::string> registry;

// Entry point
const std::string& GetNameFromRegistryByTraits(const std::vector<std::string>& rawRegistry, const std::string& soughtTraits)
{
    const static std::string error_string = "Error";
    try {
        registry reg;

        for(const auto& it: rawRegistry) {
            reg.add(person::from_string(it));
        }

        auto pp = person::from_string(soughtTraits);
        const auto* p = reg.find(pp.get_traits());
        return p != nullptr ? p->get_trait("Name") : person::unknown_person().get_trait("Name");
    } catch(const std::runtime_error& e) {
        return error_string; 
    }
}


int main(int, char**) {
    std::cout << GetNameFromRegistryByTraits({
		"Name=Mirjam Sex=Woman Age=28 Country=BE",
		"Name=Johnny Sex=Man Age=8 Country=UK",
		"Name=Anna Sex=Woman Age=71 Country=SW",
		"Name=Fred Sex=Man Age=51 Country=NL",
		"Name=Frederique Sex=Woman Age=51 Country=FR=%^^R!2"
	}, "Age=51 Sex=Man")  << std::endl;
    return 0;
}
