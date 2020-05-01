#include <functional>

union SDL_Event;

namespace wee {
    namespace detail {
        class SDL_EventLoop { 
        public:
            typedef std::chrono::high_resolution_clock timer;
            typedef std::chrono::milliseconds ms;
            typedef std::chrono::duration<int> msec;

        	using fn_conditional = std::function<int(SDL_Event&)>;
	        using fn_unconditional = std::function<void(long int*, long int)>;
        private:
            fn_conditional _conditional;
            fn_unconditional _unconditional;
	    public:
    	    SDL_EventLoop(fn_conditional, fn_unconditional) noexcept;
        
		    virtual int start();


        };
    }

}
