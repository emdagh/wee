#include "picoro.h"
#include <iostream>
#include <csetjmp>
#include <cassert>
#include <functional>

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


struct coro {
    coro*       next;
    jmp_context state;
};

static coro first;
static coro* running = &first;
static coro* idle = NULL;

int resumable(coro* c) {
    return c != NULL && c->next == NULL;
}

static void push(coro** lst, coro* c) {
    c->next = *lst;
    *lst = c;
}

static coro* pop(coro** lst) {
    coro* c = *lst;
    assert(c != NULL);
    assert(*lst != NULL);
    if(*lst != NULL) *lst = c->next;
    if(c != NULL) c->next = NULL;
    return c;
}

static void* pass(coro* me, void* arg) {
    static void* saved;
    saved = arg;
    if(!setjmp(me->state.buf)) {
        longjmp(running->state.buf, 1);
    }
    return saved;
}

void* resume(coro* c, void* arg) {
    assert(resumable(c));
    push(&running, c);
    return pass(c->next, arg);
}

void* yield(void* arg) {
    return pass(pop(&running), arg);
}

void coroutine_start(size_t);
void coroutine_main(void*, size_t);

coro* coroutine(corofunc fun, size_t stacksize) {
    if(idle == NULL && !setjmp(running->state.buf)) {
        coroutine_start(stacksize);
    }
    return (coro*)(resume(pop(&idle), (void*)fun));
}
void coroutine_main(void* ret, size_t stacksize) {
    corofunc fun;
    coro me;
    push(&idle, &me);
    fun = (corofunc)pass(&me, ret);
    if(!setjmp(running->state.buf)) {
        coroutine_start(stacksize);
    }

    for(;;) {
        ret = fun(yield(&me));
        push(&idle, pop(&running));
        fun = (corofunc)pass(&me, ret);
    }

}
void coroutine_start(size_t stacksize) {
    char stack[stacksize];
    coroutine_main(stack, stacksize);
}

int main(int /* unused */, char** /* unused */) {

    std::cout << "sizeof(void*)=" << sizeof(void*) << std::endl;
    std::cout << "sizeof(intptr_t)=" << sizeof(intptr_t) << std::endl;
    std::cout << "sizeof(int32_t)=" << sizeof(int32_t) << std::endl;

    coro* c1= coroutine([] (void*) {
        for(int x=0; x < 10; x++) {
            std::cout << x << std::endl;
            yield((void*)0);
        }
        return (void*)100;
    }, 8 * 1024);
    coro* c2 = coroutine([] (void* arg) {
        for(int i=0; i < 5; i++) {
            std::cout<< i << std::endl;
            yield((void*)1);
        }
        return (void*)10;//arg);
    }, 1024);

    bool is_done = false;
    while(!is_done) {
        if(resumable(c1)) 
            resume(c1, nullptr); 

        if(resumable(c2)) 
            resume(c2, nullptr);

        is_done = !resumable(c1) && !resumable(c2);
    }
    return 1;
}

