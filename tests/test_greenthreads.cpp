#include <cstdint>
#include <iostream>
#include <functional>

const size_t stacksize = 48;

struct context {
    uint64_t rsp = 0;
    uint64_t r15 = 0;
    uint64_t r14 = 0;
    uint64_t r13 = 0;
    uint64_t r12 = 0;
    uint64_t rbx = 0;
    uint64_t rbp = 0;
};

void hello() {
    std::cout << "new stack" << std::endl;
}

void gt_switch(context* n) {
    asm(
        "mov 0x00(%0), %%rsp;"
        "ret;"
        :
        :"r"(n)
        :
    );
}

template <typename T, typename... Args>
size_t address_of(std::function<T(Args...)> f) {
    typedef T(fn_t)(Args...);
    fn_t** ptr = f.template target<fn_t*>();
    return (size_t)*ptr;
}

template <typename T>
auto decorate(T f) {
    return [f] (auto&&... args) mutable {
        f(f, std::forward<decltype(args)>(args)...);
    };
}

/**
 * TODO: look into packaged tasks to wrap std::function pointers.
 * threadpool.hpp perhaps?
 */

int main(int /**/, char** /**/) {
    //void(*pfn_hello)() = &hello;
    context ctx;
    char* stack = (char*)malloc(stacksize);


    *(uint64_t*)&stack[stacksize - 16] = (uint64_t)&hello;
    ctx.rsp = (uint64_t)&stack[stacksize - 16];
    gt_switch(&ctx);
	return 0;
}
