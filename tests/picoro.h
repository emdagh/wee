#ifndef PICORO_H
#define PICORO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct coro coro;
typedef struct coro* corohandle;
typedef void*(*corofunc)(void*);

corohandle coroutine(corofunc, size_t);
int resumable(corohandle);
void* resume(corohandle, void*);
void* yield(void*);

#ifdef __cplusplus
}
#endif
#endif
