#pragma once

#include <wee/weegl.h>
#include <streambuf>
#include <core/logstream.hpp>

namespace wee {
    template <
        typename T, 
        GLenum _Set, 
        GLenum _Get, 
        typename Traits = std::char_traits<T> 
    >
    struct basic_device_buffer : public std::basic_streambuf<T, Traits>
    {
        typedef T char_type;
        typedef Traits traits_type;
        typedef typename Traits::int_type int_type;
        typedef std::basic_streambuf<T, Traits> base_type;
        constexpr static int_type eof = traits_type::eof();

        struct scope {
            GLint prev;
            scope(GLuint b) {
                glGetIntegerv(_Get, &prev);
                glBindBuffer(_Set, b);
            }
            ~scope() {
                glBindBuffer(_Set, prev);
            }
        };

        GLuint _handle;
        char_type* _pbuf;
    public:
        basic_device_buffer(size_t s) 
        : _pbuf(new char_type[s]) 
        {
            glGenBuffers(1, &_handle);
            scope _(_handle);
            glBufferStorage(_Set, s, NULL, 
                GL_DYNAMIC_STORAGE_BIT //| GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
            );
            this->setp(_pbuf, _pbuf + s - 1); 
        }

        ~basic_device_buffer() {
            sync();
            delete[] _pbuf;
            glDeleteBuffers(1, &_handle);
        }

        virtual int_type overflow(int_type c = traits_type::eof()) override {

            if(traits_type::eq_int_type(c, eof)) {
                return traits_type::not_eof(c);
            } else {
                *this->pptr() = c;
                this->pbump(1);
            }
            return flush() == traits_type::eq_int_type(c, eof);
        }
        
        int flush() {
            int n = base_type::pptr() - base_type::pbase();
            {
                scope with(_handle);
                //glBufferData(GL_ARRAY_BUFFER, n, NULL, GL_DYNAMIC_DRAW);
                glBufferSubData(_Set, 0, n, (const GLvoid*)_pbuf);
            }
            base_type::pbump(-n);
            return n;
        }

        virtual int sync() override {
            return flush() == eof ? eof : 0; 
        }
    };

}
