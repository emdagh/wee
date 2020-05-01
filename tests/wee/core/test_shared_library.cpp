#include <cstdint>
#include <filesystem>
#include <dlfcn.h>
#include <exception>

namespace fs = std::filesystem;

namespace wee {
    enum class shared_library_openmode : uint8_t {
        lazy,
        now,
        global,
        local,
        nodelete,
        noload,
        deepbind
    };
    class shared_library {
        void* _handle;
    public:
        shared_library() 
        : _handle(nullptr) 
        {
        }
        virtual ~shared_library() { 
            close(); 
        }
        void open(const fs::path& p, const shared_library_openmode& mode) {
            _handle = dlopen(p.c_str());
            if(!is_open()) {
                throw std::runtime_error(dlerror());
            }
        }
        void close() {
            if(is_open()) {
                dlclose(_handle);
                _handle = nullptr; 
            }
        }
        bool is_open() const {
            return _handle != nullptr;
        }

        template <typename T>
        T* sym(const std::string& s) {
            return static_cast<T*>(dlsym(_handle, s.c_str());
        }

    };
}
