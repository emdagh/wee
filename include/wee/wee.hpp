#pragma once

#if defined(_MSC_VER)
# define __aligned_type(x)  __declspec(align(x))
# define __public_type      __declspec(dllexport)
# define __private_type
//# define __forceinline      __forceinline
# define __noinline         __declspec(noinline)
#elif defined(__GNUC__)

# define __aligned_type(x)  __attribute__((aligned(x)))
# define __public_type      __attribute__((visibility("default")))
# define __private_type     __attribute__((visibility("hidden")))
# define __forceinline      __attribute__((always_inline))
# define __noinline         __attribute__((noinline))
# define __debugbreak()
#else
# define __aligned_type(x)
# define __public_type
# define __private_type
#endif

#define __declare_aligned(T, n)     T __aligned_type(n)
#define __export                    __public_type


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&) = delete;        \
void operator=(TypeName) = delete


#if defined(__cplusplus)
#define APICALL extern "C" 
#else 
#define APICALL 
#endif

