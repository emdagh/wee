#include <iostream>

namespace wee  {
template <typename T>
class strict {
    T _v;
public:
    /*explicit strict(T value) : _v(value) {
        std::cout << "Foo: value=" << _v << std::endl;
    }*/

    explicit strict(const T& value) : _v(value) {
        std::cout << "Foo: value=" << _v << std::endl;
    }

    template <typename U>
    explicit strict(U value) = delete;

    operator T() const { return _v; }
};

template <typename T> 
strict<T> make_strict(T t)
{
    return strict<T> {t};
}
}
/*
struct foo {};
struct bar : public foo {};

int main(int, char**)
{
    
    //[[maybe_unused]] strict<bar*> ti = strict<bar*>(new foo());
    [[maybe_unused]] strict<foo*> tj = strict<foo*>(new foo());
    //[[maybe_unused]] strict<bar*> tk = make_strict(new foo());
    return 0;
}
*/
