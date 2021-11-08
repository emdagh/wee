#include <csetjmp>
#include <iostream>
#include <functional>

namespace wee {

class jmp_context {
    jmp_context* _prev;
    static jmp_context* _current;
public:
    static jmp_context* current() {
        if(_current == nullptr) {
            _current = new jmp_context;
        }
        return _current;
    }
public:
    jmp_context() {
        _prev = _current;
        _current = this;
    }

    ~jmp_context() {
        _current = _prev;
    }
public:
    jmp_buf buf;
};
jmp_context* jmp_context::_current = nullptr;
} // wee


struct coroutine {
    wee::jmp_context callee_context;
    wee::jmp_context caller_context;
    enum { WORKING = 1, DONE };
    int state;
};

void start(coroutine* c, void*(*fun)(void*), void* arg, void* sp);
void yield(coroutine* c);
int resume(coroutine* c);

int main(int, char**) {

    return 0;
}
