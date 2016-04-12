#include <Python.h>
#include <numpy/arrayobject.h>
#include "fht.h"

#define UNUSED(x) (void)(x)

static char module_docstring[] = "test 1";
static char fht_docstring[] = "test 2";

static PyObject *ffht_fht(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
  {"fht", ffht_fht, METH_VARARGS, fht_docstring},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_ffht(void) {
  PyObject *m = Py_InitModule3("_ffht", module_methods, module_docstring);
  if (!m) return;

  import_array();
}

static PyObject *ffht_fht(PyObject *self, PyObject *args) {

  UNUSED(self);

  PyObject *buffer_obj;
  int chunk_size;
  
  if (!PyArg_ParseTuple(args, "Oi", &buffer_obj, &chunk_size)) {
    return NULL;
  }

  if (chunk_size < 8 || (chunk_size & (chunk_size - 1))) {
    PyErr_SetString(PyExc_ValueError, "chunk_size must be a power of two no less than 8");
    return NULL;
  }

  PyArray_Descr *dtype;
  int ndim;
  npy_intp dims[NPY_MAXDIMS];
  PyArrayObject *arr = NULL;
  
  if (PyArray_GetArrayParamsFromObject(buffer_obj, NULL, 1, &dtype, &ndim, dims, &arr, NULL) < 0) {
    return NULL;
  }

  if (arr == NULL) {
    PyErr_SetString(PyExc_TypeError, "not a numpy array");
    return NULL;
  }

  dtype = PyArray_DESCR(arr);

  if (dtype->type_num != NPY_FLOAT && dtype->type_num != NPY_DOUBLE) {
    PyErr_SetString(PyExc_TypeError, "array must consist of floats or doubles");
    Py_DECREF(arr);
    return NULL;
  }

  if (PyArray_NDIM(arr) != 1) {
    PyErr_SetString(PyExc_TypeError, "array must be one-dimensional");
    Py_DECREF(arr);
    return NULL;
  }

  int n = PyArray_DIM(arr, 0);

  if (n == 0 || (n & (n - 1))) {
    PyErr_SetString(PyExc_ValueError, "array's length must be a power of two");
    Py_DECREF(arr);
    return NULL;
  }

  void *raw_buffer = PyArray_DATA(arr);
  if ((size_t)raw_buffer % 32) {
    PyErr_SetString(PyExc_ValueError, "array is not aligned");
    Py_DECREF(arr);
    return NULL;
  }
  int res;
  if (dtype->type_num == NPY_FLOAT) {
    float *buffer = (float*)raw_buffer;
    res = FHTFloat(buffer, n, chunk_size);
  }
  else {
    double *buffer = (double*)raw_buffer;
    res = FHTDouble(buffer, n, chunk_size);
  }

  if (res) {
    PyErr_SetString(PyExc_RuntimeError, "FHT did not work properly");
    Py_DECREF(arr);
    return NULL;
  }

  Py_DECREF(arr);

  return Py_BuildValue("");
}
