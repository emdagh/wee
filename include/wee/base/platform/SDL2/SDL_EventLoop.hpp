#include <functional>

union SDL_Event;

namespace wee {
    namespace detail {
        class SDL_EventLoop { 
        	using fn_conditional = std::function<int(SDL_Event&)>;
	        using fn_unconditional = std::function<void(void)>;

            fn_conditional _conditional;
            fn_unconditional _unconditional;
	    public:
    	    SDL_EventLoop(fn_conditional, fn_unconditional) noexcept;
        
		    virtual int start();

        };
    }

}
