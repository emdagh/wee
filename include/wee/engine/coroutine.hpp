#pragma once

namespace wee {

	class coroutine_ref;
	class coroutine {
		friend class coroutine_ref;
		int _value;
	public:
		coroutine() :
			_value(0)
		{
		}
		bool complete() const {
			return _value == -1;
		}
		void reset() {
			_value = 0;
		}
	};

	class coroutine_ref {
		int _value;
		bool _dirty;
	public:
		coroutine_ref(coroutine& c) :
				_value(c._value), _dirty(false){
		}
		coroutine_ref(coroutine* c) :
			_value(c->_value), _dirty(false){
		}
		~coroutine_ref()
		{
			if (!_dirty)
				_value = -1;
		}
		operator int() const
		{
			return _value;
		}
		int operator=(int v) 
		{
			_dirty = true;
			return _value = v;
		}
		void operator=(const coroutine_ref&);
	};


#define CORO_REENTER(c) \
	switch(wee::coroutine_ref __coro = c) \
	case -1: if(__coro) { \
				 goto __terminate_coroutine;\
				 __terminate_coroutine:\
				 __coro = -1;\
				 goto __bail_out_of_coroutine;\
				 __bail_out_of_coroutine:\
				 break; \
			 }\
	else case 0:


#define CORO_YIELD(c) \
	for(__coro = c;;) \
	if(__coro == 0) { \
		case c: ;\
				break; \
	} else \
	switch(__coro ? 0 : 1) \
	for(;;) \
	case -1: if(__coro) goto __bail_out_of_coroutine; \
	else case 0:


#define CORO_FORK_IMPL(c) \
	for (_coro_value = -c;; _coro_value = c) \
	if (_coro_value == c) \
	{ \
		case -__LINE__: ; \
		break; \
	} \
	else

}
