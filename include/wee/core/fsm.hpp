#include <optional>
#include <variant>

template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
}; // (1)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>; // (2)

#if 1

struct ignore_unknown_event
{
    template <typename State, typename Event> auto operator()(const State&, const Event&)
    {
        return std::nullopt;
    }
};

template <typename T, typename OnInvalidEvent> struct basic_fsm
{
    T _state;

public:
    template <typename E> void dispatch(E&& event)
    {
        // DEBUG_METHOD();

        auto new_state = std::visit(overloaded{[&](auto& s, decltype(s.on_event(event))* = nullptr) -> std::optional<T> {
                                                   return s.on_event(std::forward<E>(event));
                                               },
                                               [&](auto&... s) -> std::optional<T> {
                                                   return OnInvalidEvent{}(s..., std::forward<E>(event));
                                               }},
                                    _state);
        if(new_state)
        {
            _state = *std::move(new_state);
        }
    }
};
