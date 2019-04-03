#pragma once

#include <iostream>

namespace wee {
    class binary_writer {
        std::ostream& _os;
    public:
        binary_writer(std::ostream& os) : _os(os) 
        {
            
        }
    };
}
