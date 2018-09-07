#pragma once

namespace wee {

    struct frame {
        rect _rect;
        bool _rotated;
        bool _trimmed;
        rect _spritesourcesize;
    };

    struct frames {
        std::vector<frame> _frame;
    };
}
