#ifndef _FHT_H_
#define _FHT_H_

int fht_float(float *buf, int log_n);
int fht_double(double *buf, int log_n);
static inline int fht_float_oop(float *in, float *out, int log_n);
static inline int fht_double_oop(double *in, double *out, int log_n);


#ifdef __cplusplus
static inline int fht(float *buf, int log_n) {
    return fht_float(buf, log_n);
}

static inline int fht(double *buf, int log_n) {
    return fht_double(buf, log_n);
}

static inline int fht(float *buf, float *out, int log_n) {
    return fht_float_oop(buf, out, log_n);
}

static inline int fht(double *buf, double *out, int log_n) {
    return fht_double_oop(buf, out, log_n);
}
#endif // #ifdef __cplusplus

#include "fht_impl.h"

#endif
