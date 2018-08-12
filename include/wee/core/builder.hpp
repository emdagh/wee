#pragma once

namespace wee {
    template <typename T>
    class builder {

        public:
            static T builder() { return {}; }
            T& build() { return static_cast<T&>(*this); }
    };
}
