#ifndef _WEE_H_
#define _WEE_H_

#include <wee/config.h>

#if defined(_MSC_VER)
#include <Windows.h>
# define __aligned_type(x)  __declspec(align(x))
#elif defined(__GNUC__)
# define __aligned_type(x)  __attribute__((aligned(x)))
#else
# define __aligned_type(x)
#endif
#define __declare_aligned(T, n)     T __aligned_type(n)



#if defined(__cplusplus)
#define APICALL extern "C" 
#else 
#define APICALL 
#endif

#define C_API APICALL

#endif
