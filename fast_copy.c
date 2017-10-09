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
    __m512 *ov = (__m512 *)out;
    __m512 *iv = (__m512 *)in;
    size_t f = (n >> 6) >> 3, leftover = (n >> 6) & 0x7u;
    n = 0;
    switch(leftover) {
        case 0: do {
                ov[n] = iv[n]; ++n;
        case 7: ov[n] = iv[n]; ++n;
        case 6: ov[n] = iv[n]; ++n;
        case 5: ov[n] = iv[n]; ++n;
        case 4: ov[n] = iv[n]; ++n;
        case 3: ov[n] = iv[n]; ++n;
        case 2: ov[n] = iv[n]; ++n;
        case 1: ov[n] = iv[n]; ++n;
                   } while(f--);
    }
}
#elif __AVX2__
// If unaligned or n is less than 32, defaults to memcpy.
void fast_copy(void *out, void *in, size_t n) {
    if(n < 32 || ((long long)out & 31u) || ((long long)in & 31u)) {
        memcpy(out, in, n);
        return;
    }
    __m256 *ov = (__m256 *)out;
    __m256 *iv = (__m256 *)in;
    size_t f = (n >> 5) >> 3, leftover = (n >> 5) & 0x7u;
    n = 0;
    switch(leftover) {
        case 0: do {
                ov[n] = iv[n]; ++n;
        case 7: ov[n] = iv[n]; ++n;
        case 6: ov[n] = iv[n]; ++n;
        case 5: ov[n] = iv[n]; ++n;
        case 4: ov[n] = iv[n]; ++n;
        case 3: ov[n] = iv[n]; ++n;
        case 2: ov[n] = iv[n]; ++n;
        case 1: ov[n] = iv[n]; ++n;
                   } while(f--);
    }
}
#elif __SSE2__
// If unaligned or n is less than 16, defaults to memcpy
// Assumes alignment and that n is a power of 2 >= 16
void fast_copy(void *out, void *in, size_t n) {
    if(n < 16 || ((long long)out & 15u) || ((long long)in & 15u)) {
        memcpy(out, in, n);
        return;
    }
    __m128 *ov = (__m128 *)out;
    __m128 *iv = (__m128 *)in;
    size_t f = (n >> 4) >> 3, leftover = (n >> 4) & 0x7u;
    n = 0;
    switch(leftover) {
        case 0: do {
                ov[n] = iv[n]; ++n;
        case 7: ov[n] = iv[n]; ++n;
        case 6: ov[n] = iv[n]; ++n;
        case 5: ov[n] = iv[n]; ++n;
        case 4: ov[n] = iv[n]; ++n;
        case 3: ov[n] = iv[n]; ++n;
        case 2: ov[n] = iv[n]; ++n;
        case 1: ov[n] = iv[n]; ++n;
                   } while(f--);
    }
}
#else
void fast_copy(void *out, void *in, size_t n) {
        memcpy(out, in, n);
}
#endif
