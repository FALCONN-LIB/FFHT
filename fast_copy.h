#ifndef _FAST_COPY_H__
#define _FAST_COPY_H__
#include <stdlib.h>

#ifndef FAST_COPY_MEMCPY_THRESHOLD
#  define FAST_COPY_MEMCPY_THRESHOLD (1ull << 23)
#endif

#ifdef __cplusplus
extern "C" {
#endif
void *fast_copy(void *out, void *in, size_t m);
#ifdef __cplusplus
} // extern "C"
#endif

#endif // _FAST_COPY_H__
