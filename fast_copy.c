#include "fast_copy.h"
#include <string.h>
#include <stdlib.h>
#if (defined(__x86_64__) || defined(__i386__))
#  include <x86intrin.h>
#endif

// These functions all assume that the size of memory being copied is a power of 2.

#if _FEATURE_AVX512F
// If unaligned or n is less than 64, defaults to memcpy.
void fast_copy(void *out, void *in, size_t n) {
    if(n < 64 || ((long long)out & 63u) || ((long long)in & 63u)) {
        memcpy(out, in, n);
        return;
    }
    n >>= 6;
    for(__m512 *ov = (__m512 *)out, *iv = (__m512 *)in; n--; ov[n] = iv[n]);
}
#elif __AVX2__
// If unaligned or n is less than 32, defaults to memcpy.
void fast_copy(void *out, void *in, size_t n) {
    if(n < 32 || ((long long)out & 31u) || ((long long)in & 31u)) {
        memcpy(out, in, n);
        return;
    }
    //__m256 *ov = (__m256 *)out;
    //__m256 *iv = (__m256 *)in;
    n >>= 5;
    for(__m256 *ov = (__m256 *)out, *iv = (__m256 *)in; n--; ov[n] = iv[n]);
}
#elif __SSE2__
// If unaligned or n is less than 16, defaults to memcpy
// Assumes alignment and that n is a power of 2 >= 16
void fast_copy(void *out, void *in, size_t n) {
    if(n < 16 || ((long long)out & 15u) || ((long long)in & 15u)) {
        memcpy(out, in, n);
        return;
    }
    n >>= 4;
    for(__m128 *ov = (__m128 *)out, *iv = (__m128 *)in; n--; ov[n] = iv[n]);
}
#else
void fast_copy(void *out, void *in, size_t n) {
        memcpy(out, in, n);
}
#endif
