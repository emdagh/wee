#pragma once

#include <iostream>
#include <fstream>
#include <ios>
#include <zlib.h>
#include <string.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include <cassert>

namespace wee{

	template <typename T, typename Traits = std::char_traits<T> >
	class basic_ozstreambuf : public std::basic_streambuf<T, Traits> {
		typedef std::basic_streambuf<T, Traits> base_t;
		typedef std::basic_ostream<T, Traits> 	stream_t;
		typedef T 								char_t;
		typedef typename base_t::int_type 				int_t;

		static const int BUFFER_LEN 	= 4096;
		static const int BUFFER_SIZE 	= BUFFER_LEN * sizeof(T);

		stream_t* out;

		char_t pbuf[BUFFER_LEN];
	public:
		basic_ozstreambuf(stream_t& os) : out(&os) {
			memset(pbuf, 0, sizeof(char_t) * BUFFER_LEN);
			this->setp(pbuf, pbuf + BUFFER_SIZE);
			this->setg(pbuf, pbuf, pbuf);
		}

		virtual ~basic_ozstreambuf() {
			out->flush();
		}

		int_t overflow(int_t c = Traits::eof()) {

			int w = static_cast<int>(this->pptr() - this->pbase());
			if (c == Traits::eof()) {
				*base_t::pptr() = c;
				base_t::pbump(1);
			}
			if (int res = _compress(this->pbase(), w)) {
				//this->setp(this->pbase(), this->epptr() - 1);
				this->setp(pbuf, pbuf + BUFFER_SIZE);
				return c;
			} else
				return Traits::eof();
		}

	public:
		std::streamsize _compress(char_t* buf, std::streamsize size) {
			char_t dst[BUFFER_LEN] = { 0 };
			uLongf dst_size = BUFFER_LEN;
			uLongf src_size = size;
			int err = compress2(
					reinterpret_cast<unsigned char*>(dst), &dst_size,
					reinterpret_cast<unsigned char*>(buf), src_size,
					Z_BEST_SPEED + 5);
			out->write(&dst[0], dst_size);
			return dst_size;
		}

		int sync() {
			if (this->pptr() && this->pptr() > this->pbase()) {
				int c = overflow(Traits::eof());
				if (c == Traits::eof())
					return -1;
			}
			return 0;
		}
	};

	template <typename T, typename Traits = std::char_traits<T> >
	class basic_izstreambuf : public std::basic_streambuf<T, Traits> {
		typedef std::basic_streambuf<T, Traits> base_t;
		typedef std::basic_istream<T, Traits> 	stream_t;
		typedef T 								char_t;
		typedef typename base_t::int_type 		int_t;

		static const int BUFFER_LEN 	= 4096;
		static const int BUFFER_SIZE 	= BUFFER_LEN * sizeof(T);

		stream_t* in;

		char_t gbuf[BUFFER_SIZE];
	public:
		basic_izstreambuf(stream_t& os) : in(&os) {
			memset(gbuf, 0, sizeof(char_t) * BUFFER_SIZE);
			this->setp(gbuf, gbuf + BUFFER_LEN);
			this->setg(gbuf, gbuf, gbuf);
		}

		int_t underflow() {
			if (base_t::gptr() < base_t::egptr()) {
				return *base_t::gptr();
			}

			int num;
			if ((num = _decompress(gbuf, BUFFER_SIZE)) == 0) {
				return Traits::eof();
			}
			base_t::setg(gbuf, gbuf, gbuf + num);
			return *base_t::gptr();
		}
	protected:
		uLongf _decompress(char_t* buf, std::streamsize len)
		{
			char_t src[BUFFER_LEN] = { 0 };
			uint32_t src_size = BUFFER_SIZE;
			uLongf dst_size = BUFFER_SIZE;
			in->read(src, BUFFER_SIZE);
			{
				int err = uncompress(
					reinterpret_cast<unsigned char*>(gbuf), &dst_size,
					reinterpret_cast<unsigned char*>(src),   len
				);
				return dst_size;
			}
			return 0;
		}
	};


}
