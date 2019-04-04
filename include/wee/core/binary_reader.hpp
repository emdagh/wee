#pragma once

#include <iostream>


namespace wee {

    class binary_reader {
        std::istream& _is;
    public:
        explicit binary_reader(std::istream& is) : _is(is) {
        }
        virtual ~binary_reader() {
        }
        int32_t read_7bit_encoded_int() const {
            int32_t count = 0;
            int32_t shift = 0;
            using byte = uint8_t;
            byte b;
            do {
                if(shift == 5 * 7)
                    throw std::runtime_error("wrong 7-bit encoded format");
                _is.read(reinterpret_cast<char*>(&b), 1);
                count |= (b & 0x7f) << shift;
                shift += 7;
            } while ((b & 0x80) != 0);
            return count;
        }
        std::string read_string() const {
            int32_t n = read_7bit_encoded_int();
            std::string res(n, ' ');
            _is.read(&res[0], n);
            return res;
        }

        template <typename T>
        binary_reader& read(T* res) const {
            return _is.read(reinterpret_cast<char*>(res), sizeof(T)), *this;
            
        }
        
    };
}
