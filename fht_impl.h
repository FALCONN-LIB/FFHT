#ifdef __AVX__
#include "fht_avx.c"
#define VECTOR_WIDTH (32u)
#else
#include "fht_sse.c"
#define VECTOR_WIDTH (16u)
#endif
