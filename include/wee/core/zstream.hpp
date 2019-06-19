#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <zlib.h>

namespace wee {

    enum struct strategy : uint8_t {
        kFiltered,
        kHuffmanOnly,
        kDefault
    };

    template <
        typename T, 
        typename Traits = std::char_traits<T>
    >
    class basic_unzip_streambuf : public std::basic_streambuf<T, Traits> {
        z_stream _zip;
        std::vector<T> _buffer;
        long _crc;
    public:
        typedef typename Traits::int_type int_type;
    public:
        basic_unzip_streambuf(std::ostream& os, int level, size_t sz) {

            _buffer = std::vector(sz, 0);

            _zip.zalloc = static_cast<alloc_func>(nullptr);
            _zip.zfree  = static_cast<free_func>(nullptr);
            _zip.next_in    = nullptr;
            _zip.avail_in   = 0;
            _zip.next_out   = nullptr;
            _zip.avail_out  = 0;

            auto err = deflateInit(&_zip, std::max(0, std::min(9, level)));
            if(err != Z_OK) {
                throw std::runtime_error("error initializing zstream");
            }

            _zip.avail_out = static_cast<int>(_buffer.size());
            _zip.next_out  =&_buffer[0];

            char* p = &_buffer[0];
            this->setp(p, p + sz); 

        }
        virtual ~basic_unzip_streambuf();
        int sync();
        int_type overflow(int_type);
        std::streamsize zfinish();
        long crc() const;
    protected:
        bool zip_to_stream(T*, std::streamsize);


    };


    template <typename T, typename Traits = std::char_traits<T> >
    class basic_ozstream : public std::basic_ostream<T, Traits> 
    {
    };


    template <typename T, typename Traits = std::char_traits<T> >
    class basic_zip_streambuf : public std::basic_streambuf<T, Traits> 
    {
    };

    
    template <typename T, typename Traits = std::char_traits<T> >
    class basic_izstream : public std::basic_ostream<T, Traits> 
    {
    };


}
