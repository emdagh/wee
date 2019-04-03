#pragma once

namespace wee {
    enum struct texture_address_mode : uint8_t {
        kClamp,
        kMirror,
        kWrap
    };
}
