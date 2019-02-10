#pragma once

namespace wee {

    class binary_reader {
        std::istream& _is;
    public:
        explicit binary_reader(std::istream& is) : _is(is) {
        }
        virtual binary_reader() {
        }
        int32_t read_7bit_encoded_int() const {
            int32_t ret = 0;
            int index = 0;
            while(index != 35) {
                int8_t b;
                _is.read(&b, 1);
                ret |= ((int32_t)b & (int32_t)127) << index;
                index += 7;
                if(((int32_t)b & 128) == 0) {
                    return ret;
                }
            }
            return 0;
        }
        std::string read_string() const {
            int32_t n = read_7bit_encoded_int();
            std::string res(n, ' ');
            _is.read(&res[0], n);
            return res;
        }
        
    };
}
