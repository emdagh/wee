#include <base/application.hpp>

using namespace wee;

struct application_create : public state<application> {};
struct application_start : public state<application> {};
struct application_resume : public state<application> {};
struct application_pause: public state<application> {};
struct application_stop : public state<application>{};
struct application_destroy : public state<application>{};



