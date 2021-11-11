#pragma once

namespace wee {
template <typename> class decorator;
template <typename T, typename... Ts>
class decorator<T(Ts...)>
{
  std::function<T(Ts...)> _f;
public:
  decorator(std::function<T(Ts...)> f) : _f(f) {}
  T operator () (Ts... ts)
  {
    return _f(ts);
  }
};

template <typename T, typename... Ts>
decorator<T(Ts...)> make_decorator(T(*f)(Ts...))
{
  return decorator<T(Ts...)>(std::function<R(Args...)>(f));
}
}
