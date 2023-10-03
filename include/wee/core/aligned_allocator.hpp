#include <cstdint>
#include <limits>
#include <memory>
#include <new>
#include <vector>

template <typename T, unsigned A>
struct aligned_allocator {
    using value_type = T;
    std::align_val_t alignment { A };

    aligned_allocator(){}

    template <typename U>
    aligned_allocator(const aligned_allocator<U, A>&) noexcept {}

    template<typename U>
    struct rebind  { using other = aligned_allocator<U, A>; };

    [[nodiscard]] 
    T* allocate(std::size_t n) const {
        return reinterpret_cast<T*>(operator new[] ( n * sizeof(T), std::align_val_t { A }));
    }
    constexpr void deallocate(T* p, size_t) const {
        ::operator delete[](p, std::align_val_t { A });
    }
    constexpr bool operator == (const aligned_allocator&) const { return true; }
    constexpr bool operator != (const aligned_allocator&) const { return false; }
    
};
