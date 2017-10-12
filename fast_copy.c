#include "fast_copy.h"
#include <string.h>
#include <stdlib.h>
#if (defined(__x86_64__) || defined(__i386__))
#  include <x86intrin.h>
#endif

// These functions all assume that the size of memory being copied is a power of 2.

#if _FEATURE_AVX512F
// If n is less than 64, defaults to memcpy. Otherwise, being a power of 2, we can just use unaligned stores and loads.
void fast_copy(void *out, void *in, size_t n) {
    if(n & 63) {
        memcpy(out, in, n);
        return;
    }
    n >>= 6;
    for(__m512 *ov = (__m512 *)out, *iv = (__m512 *)in; n--;) {
        _mm512_storeu_ps((float *)(ov++), _mm512_loadu_ps((float *)(iv++)));
    }
}
#elif __AVX2__
// If n is less than 32, defaults to memcpy. Otherwise, being a power of 2, we can just use unaligned stores and loads.
void fast_copy(void *out, void *in, size_t n) {
    if(n & 32) {
        memcpy(out, in, n);
        return;
    }
    n >>= 5;
    for(__m256 *ov = (__m256 *)out, *iv = (__m256 *)in; n--;) {
        _mm256_storeu_ps((float *)(ov++), _mm256_loadu_ps((float *)(iv++)));
    }
}
#elif __SSE2__
// If n is less than 16, defaults to memcpy. Otherwise, being a power of 2, we can just use unaligned stores and loads.
void fast_copy(void *out, void *in, size_t n) {
    if(n & 15u) {
        memcpy(out, in, n);
        return;
    }
    n >>= 4;
    for(__m128 *ov = (__m128 *)out, *iv = (__m128 *)in; n--;) {
        _mm_storeu_ps((float *)(ov++), _mm_loadu_ps((float *)(iv++)));
    }
}
#else
void fast_copy(void *out, void *in, size_t n) {
        memcpy(out, in, n);
}
#endif
