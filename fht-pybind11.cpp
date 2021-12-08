#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "fht.h"

namespace py = pybind11;

static constexpr inline unsigned ilog2(size_t x) noexcept {
    return __builtin_ctz(x);
}

template<typename T, int flags>
py::array_t<T> npfht_oop(py::array_t<T, flags> input) {
    auto bi = input.request();
    if(bi.ndim != 1) throw std::invalid_argument("input must be 1-d array");
    const size_t n = bi.size;
    if(!n || (n & (n - 1)) != 0u) {
        throw std::invalid_argument("Input must be power-of-2 sized");
    }
    if(sizeof(T) != bi.itemsize) {
        std::fprintf(stderr, "Error: Corrupted NumPy array (itemsize does not match data type).\n");
        throw std::runtime_error("Error: Corrupted NumPy array (itemsize does not match data type).");
    }
    py::array_t<T> ret(n);
    py::buffer_info rbi = ret.request();
    if(::fht((T *)bi.ptr, (T *)rbi.ptr, ilog2(n))) {
        throw std::runtime_error("FHT did not work properly");
    }
    return ret;
}

template<typename T, int flags>
py::array_t<T> npfht2d_oop(py::array_t<T, flags> input, int nthreads=1) {
    auto bi = input.request();
    if(bi.ndim != 2) throw std::invalid_argument("input must be 2-d array");
    const py::ssize_t n = bi.shape[1];
    if(!n || (n & (n - 1)) != 0u) {
        throw std::invalid_argument("Input must be power-of-2 dimension");
    }
    const py::ssize_t nrows = bi.shape[0];
    if(sizeof(T) != bi.itemsize) {
        std::fprintf(stderr, "Error: Corrupted NumPy array (itemsize does not match data type).\n");
        throw std::runtime_error("Error: Corrupted NumPy array (itemsize does not match data type).");
    }
    py::array_t<T> ret(std::vector<py::ssize_t>{nrows, n});
    py::buffer_info rbi = ret.request();
    std::vector<int8_t> rvs(nrows);
    const int il2 = ilog2(n);
    #pragma omp parallel for num_threads(nthreads)
    for(size_t i = 0; i < nrows; ++i) {
        rvs[i] = ::fht((T *)bi.ptr + i * n, (T *)rbi.ptr + i * n, il2);
    }
    if(std::any_of(rvs.begin(), rvs.end(), [](int8_t x) {return x != 0;})) {
        throw std::runtime_error("FHT did not work properly");
    }
    return ret;
}

template<typename T, int flags>
py::array_t<T> npfht2d_ip(py::array_t<T, flags> input, int nthreads=1) {
    auto bi = input.request();
    if(bi.ndim != 2) throw std::invalid_argument("input must be 2-d array");
    const py::ssize_t n = bi.shape[1];
    if(!n || (n & (n - 1)) != 0u) {
        throw std::invalid_argument("Input must be power-of-2 dimension");
    }
    const py::ssize_t nrows = bi.shape[0];
    if(sizeof(T) != bi.itemsize) {
        std::fprintf(stderr, "Error: Corrupted NumPy array (itemsize does not match data type).\n");
        throw std::runtime_error("Error: Corrupted NumPy array (itemsize does not match data type).");
    }
    std::vector<int> rvs(nrows);
    const int il2 = ilog2(n);
    #pragma omp parallel for num_threads(nthreads)
    for(size_t i = 0; i < nrows; ++i) {
        rvs[i] = ::fht((T *)bi.ptr + i * n, (T *)bi.ptr + i * n, il2);
    }
    if(std::any_of(rvs.begin(), rvs.end(), [](int8_t x) {return x != 0;})) {
        throw std::runtime_error("FHT did not work properly");
    }
    return input;
}



template<typename T, int flags>
py::array_t<T, flags> npfht_ip(py::array_t<T, flags> input) {
    auto bi = input.request();
    T *ptr = (T *)bi.ptr;
    if(bi.ndim != 1) throw std::invalid_argument("input must be 1-d array");
    const size_t n = bi.size;
    if(!n || (n & (n - 1)) != 0u) {
        throw std::invalid_argument("Input must be power-of-2 sized");
    }
    if(sizeof(T) != bi.itemsize) {
        std::fprintf(stderr, "Error: Corrupted NumPy array (itemsize does not match data type).\n");
        throw std::runtime_error("Error: Corrupted NumPy array (itemsize does not match data type).");
    }
    if(::fht(ptr, ilog2(n))) {
        throw std::runtime_error("FHT did not work properly");
    }
    return input;
}

py::array npfht(py::array input, py::ssize_t nthreads=1) {
    py::buffer_info bi = input.request();
    if(bi.ndim > 2) throw std::runtime_error("ndim > 2. Only accepted: 1d and 2d numpy arrays");
    if(bi.format.front() == 'f') {
        py::array_t<float, py::array::forcecast> farr(input);
        if(bi.ndim == 2)
            return npfht2d_oop(farr, nthreads);
        return npfht_oop(farr);
    } else {
        py::array_t<double, py::array::forcecast> farr(input);
        if(bi.ndim == 2)
            return npfht2d_oop(farr, nthreads);
        return npfht_oop(farr);
    }
}

py::array npfht_(py::array input, py::ssize_t nthreads=1) {
    py::buffer_info bi = input.request();
    if(bi.format.front() == 'f') {
        py::array_t<float, py::array::forcecast> farr(input);
        if(bi.ndim == 2)
            return npfht2d_ip(farr, nthreads);
        return npfht_ip(farr);
    } else {
        py::array_t<double, py::array::forcecast> farr(input);
        if(bi.ndim == 2)
            return npfht2d_ip(farr, nthreads);
        return npfht_ip(farr);
    }
}




static char module_docstring[] =
    "A C extension that computes the Fast Hadamard Transform";
static char fht_docstring[] =
    "Compute the Fast Hadamard Transform (FHT) for a given "
    "one-dimensional NumPy array.\n\n"
    "The Hadamard Transform is a linear orthogonal map defined on real vectors "
    "whose length is a _power of two_. For the precise definition, see the "
    "[Wikipedia entry](https://en.wikipedia.org/wiki/Hadamard_transform). The "
    "Hadamard Transform has been recently used a lot in various machine "
    "learning "
    "and numerical algorithms.\n\n"
    "The implementation uses "
    "[AVX](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions) "
    "to speed up the computation. If AVX is not supported on your machine, "
    "a simpler implementation without (explicit) vectorization is used.\n\n"
    "The function takes two parameters:\n\n"
    "* `buffer` is a NumPy array which is being transformed. It must be a "
    "one-dimensional array with `dtype` equal to `float32` or `float64` (the "
    "former is recommended unless you need high accuracy) and of size being a "
    "power "
    "of two. If your CPU supports AVX, then `buffer` must be aligned to 32 "
    "bytes. "
    "To allocate such an aligned buffer, use the function `created_aligned` "
    "from this "
    "module.\n"
    "* `chunk` is a positive integer that controls when the implementation "
    "switches "
    "from recursive to iterative algorithm. The overall algorithm is "
    "recursive, but as "
    "soon as the vector becomes no longer than `chunk`, the iterative "
    "algorithm is "
    "invoked. For technical reasons, `chunk` must be at least 8. A good choice "
    "is to "
    "set `chunk` to 1024. But to fine-tune the performance one should use a "
    "program "
    "`best_chunk` supplied with the library.\n";

PYBIND11_MODULE(ffht, m) {
    m.doc() = module_docstring;
    m.def("fht", npfht, py::arg("input"), py::arg("nthreads") = 1, fht_docstring);
    m.def("fht_", npfht_, py::arg("input"), py::arg("nthreads") = 1, "In-place version of ffht.fht. See ffht.fht documentation for details");
}
