#ifdef __AVX__
#include "fht_avx.c"
#define VECTOR_WIDTH (32u)
#else
#include "fht_sse.c"
#define VECTOR_WIDTH (16u)
#endif

#if !_ISOC11_SOURCE
#  if !_POSIX_C_SOURCE >= 200112L && !_XOPEN_SOURCE >= 600
#    error("Need either ISOC11 or POSIX support for aligned allocation.")
#  endif
void *aligned_alloc(size_t size, size_t alignment) {
    void *ret;
    if(posix_memalign(&ret, size, alignment)) return 0;
    return ret;
}
#endif

int fht_float_oop(float *in, float *out, int log_n) {
    if(out == 0) {
        if((out = aligned_alloc(sizeof(float) << log_n, VECTOR_WIDTH)) == 0) return -2;
        memcpy(out, in, sizeof(float) << log_n);
    }
    return fht_float(in, out, log_n);
}

int fht_double_oop(double *in, double *out, int log_n) {
    if(out == 0) {
        if((out = aligned_alloc(sizeof(double) << log_n, VECTOR_WIDTH)) == 0) return -2;
        memcpy(out, in, sizeof(double) << log_n);
    }
    return fht_double(in, out, log_n);
}

