from _ffht import fht
import numpy as _numpy

def create_aligned(n, dtype, alignment=32):
    buf = _numpy.zeros(n + alignment, dtype=dtype)
    if alignment % buf.itemsize:
        raise ValueError('alignment must be a multiple of the size of dtype')
    off = buf.ctypes.data % alignment
    shift = 0
    if off != 0:
        shift = (alignment - off) / buf.itemsize
    return buf[shift : shift + n]
