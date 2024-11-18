#include <functional>
#include <type_traits>
#include <thread>
#include <chrono>
#include <iostream>
#include <iostream>
#include <iomanip>
#include <ctime>

using namespace std::literals;

static constexpr int BACKOFF_OK = 0;

template <typename... Ts>
void debug_info(Ts&&... ts) {
    //auto now = std::chrono::system_clock::now();
    //auto time_point = std::chrono::system_clock::to_time_t(now);
    //std::cout << std::ctime(&time_point);
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();


    std::time_t time_point = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&time_point), "%F %T") << "." << std::setw(3) << std::setfill('0') << ms % 1000 << " ";
    
    ((std::cout << ts << " "), ...);
    std::cout << std::endl;
}

#define DEBUG_INFO(...) debug_info(__VA_ARGS__)

template <typename Fun, typename Duration>
auto backoff_timer(Fun&& func, Duration initial_delay, float backoff_factor = 2.f, int max_retries=5) {
    using return_t = std::invoke_result_t<Fun>;
    
    return [=] (auto&&... args) mutable -> return_t {
        int retries = 0;
        auto delay = initial_delay;
        while(retries++ < max_retries) {
            DEBUG_INFO("here...");
            return_t ret = func(std::forward(args)...);
            if(ret == static_cast<return_t>(BACKOFF_OK)) { // assuming 0 == success...
                return ret;
            }
            std::this_thread::sleep_for(delay);
            delay *= backoff_factor;
        }
        return return_t {};
    };
}
/**
int fun() {
    return -1;
}

int main() {
    auto tt = backoff_timer(fun, 500ms);
    tt();
    return 0;
}
*/
