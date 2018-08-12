#pragma once

template <typename T, typename Traits = std::char_traits<T> >
class basic_teebuf : public std::basic_streambuf<T, Traits> {
    typedef typename Traits::int_type int_type;
    typedef std::basic_streambuf<T, Traits> streambuf_type;

    streambuf_type* _s0;
    streambuf_type* _s1;
    protected:
    virtual int overflow(int_type c) {
        if(c == Traits::eof()) {
            return !Traits::eof();
        }
        int r1 = _s0->sputc(c);
        int r2 = _s1->sputc(c);

        return r1 == Traits::eof() || r2 == Traits::eof();
    }

    virtual int sync() {
        int r1 = _s0->pubsync();
        int r2 = _s1->pubsync();
        return r1 == 0 && r2 == 0 ? 0 : -1;
    }
    public:
    basic_teebuf(streambuf_type* s0, streambuf_type* s1) 
        : _s0(s0)
          , _s1(s1) 
    {
    }
};


template <typename T, typename Traits = std::char_traits<T> >
class basic_teestream : public std::basic_ostream<T, Traits> {
    typedef std::basic_ostream<T, Traits> stream_type;
    typedef basic_teebuf<T, Traits> streambuf_type;
    streambuf_type _buf;
    public:
    basic_teestream(stream_type& s0, stream_type& s1) 
        : stream_type(&_buf)
          , _buf(s0.rdbuf(), s1.rdbuf())
    {
    }
};
