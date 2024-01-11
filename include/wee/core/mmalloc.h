#pragma once

#include <memory_resource>
#include <iostream>
#include <type_traits>
#include <atomic> 
#include <cassert>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <iterator>

namespace wee {

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
T align_up(T n, T a) { return (n + a) & ~(a-1); }

class mmap_arena {
    void*   _mem;
    size_t  _size;
public:
    mmap_arena(size_t size) : _size(size) {
        _mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (_mem == MAP_FAILED) {
            throw std::bad_alloc();
        }
        char* tail = (char*)_mem + _size - getpagesize(); // use sysconf(_SC_PAGESIZE)  for portable applications
        mprotect(tail, getpagesize(), PROT_NONE);
    }

    virtual ~mmap_arena() {
        munmap(_mem, _size);
    }

    void* data()    { return _mem; }
    size_t size()   { return _size; }
};

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
bool is_power_of_two(T x) {
    return (x & (x - 1)) == 0;
}

class short_memory_resource : public std::pmr::memory_resource {
    constexpr static const size_t _alignment = sizeof(max_align_t);
    
    alignas(_alignment) char* _mem;
    char* _ptr;
    size_t _size;
public:
    constexpr static const size_t alignment = _alignment;

    short_memory_resource(void* mem, size_t size) 
    : _mem((char*)mem)
    , _ptr(_mem)
    , _size(size) 
    {
        
    }
    virtual void* do_allocate(size_t size, size_t a) {
        assert(is_power_of_two(a));
        assert(a <= _alignment);

        size = align_up<size_t>(size, _alignment);

        if(static_cast<size_t>(_mem + _size - _ptr) >= size) {
            char* r = _ptr;
            _ptr += size;
            return r;
        }
        
        return NULL;
    }

    virtual void do_deallocate(void* p, size_t size, size_t /*a*/) {
        size = align_up<size_t>(size, _alignment);
        char* ptr = (char*)p;
        if(ptr + size == _ptr) {
            _ptr = ptr;
        }
    }

    virtual bool do_is_equal(const std::pmr::memory_resource& /*other*/) const noexcept {
        return false;
    }
};

template <typename T, typename R = short_memory_resource>
class short_alloc {
    R* _res;   
    static_assert(alignof(T) < R::alignment);
public:
    using value_type = T;
   
    template <typename U>
    struct rebind { using other = short_alloc<U>; };

    short_alloc(R& r) : _res(&r) {}

    [[nodiscard]]
    T* allocate(size_t n) {
        
        return (T*)_res->allocate(n * sizeof(T), alignof(T));
    }

    void deallocate(T* p, size_t n) {
        _res->deallocate((void*)p, n, alignof(T));
    }
};
} // wee
/*
namespace mm {
    template <typename T>
    using vector = std::vector<T, short_alloc<T> >;
}

std::string sys(const char* c) {
    static int BUFFER_SIZE = 1024;
    FILE *fp;
    char buffer[BUFFER_SIZE];

    fp = popen(c, "r");
    if (fp != NULL)
    {
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
            printf("%s", buffer);
        pclose(fp);
    }
    return std::string(buffer);
}

int main(int, char**) {

    std::cout << sys("/proc/sys/vm/overcommit_memory") << std::endl; // hmmm..
        
    unsigned long long size = getpagesize() * 100'000; // check overcommit limit
    mmap_arena arena { size }; 
    short_memory_resource res { arena.data(), arena.size() };
    short_alloc<int> alloc { res };
    
    mm::vector<int> mv { alloc };
    for(int i=0; i < 1024; i++) {
        mv.push_back(i);
    }

    std::copy(mv.begin(), mv.end(), std::ostream_iterator<int> { std::cout, ", " });
    return 0;
}
*/
