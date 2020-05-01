#pragma once

#include <iostream>

namespace wee {
    class binary_writer {
        std::ostream& _os;
    public:
        binary_writer(std::ostream& os) : _os(os) 
        {
            
        }

        std::streampos tell() {
            return _os.tellp();

        }

        void seek(std::streamoff off, std::ios_base::seekdir way) {
            _os.seekp(off, way);
        }

        void write_7bit_encoded_int(int32_t i) {
            using unsigned_type = typename std::make_unsigned<int32_t>::type;
            unsigned_type num = static_cast<unsigned_type>(i);
            while(num >= 0x80) {
                char b = static_cast<char>(num | 0x80);
                _os.write(reinterpret_cast<const char*>(&b), 1);
                num >>= 7;
            }
            char b = static_cast<char>(num);
            _os.write(reinterpret_cast<const char*>(&b), 1);
        }

        void write_string(const std::string& s) {
            write_7bit_encoded_int(static_cast<uint32_t>(s.length()));
            _os.write(s.c_str(), s.length());
        }


        template <typename T>
        binary_writer& write(const T& t) {
            return  _os.write(reinterpret_cast<const char*>(&t), sizeof(T)), *this;
        }

    };
}
