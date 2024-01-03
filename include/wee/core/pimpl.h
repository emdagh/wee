#include <memory>

namespace wee {
template <typename T> 
class pimpl {
	std::unique_ptr<T> m;
private:
	pimpl(T* t)
		: m(t)
	{}

public:
	pimpl() : pimpl(new T{})
	{}
	pimpl(const pimpl& other)
		: m(new T(*other.m))
	{}
	pimpl(pimpl&&) = default;
	template <typename... Args>
	pimpl(Args&&... args)
		: m(std::make_unique<T>(std::forward<Args>(args)...))
	{}
	~pimpl() = default;

	T* operator -> () { return m.get(); }
	T& operator  * () { return *m.get(); }

	T* operator -> () const { return m.get(); }
	T& operator  * () const { return *m.get(); }

};
}
