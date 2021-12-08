import sys

try:
    import pypandoc
    long_description = pypandoc.convert('README.md', 'rst')
except(IOError, ImportError):
    long_description = open('README.md').read()

try:
    from setuptools import setup, find_packages, Extension
except ImportError:
    sys.stderr.write('Setuptools not found!\n')
    raise

try:
    import numpy as np
except ImportError:
    sys.stderr.write('NumPy not found!\n')
    raise

module = Extension('ffht',
                   sources=["fht-pybind11.cpp", "fht.c", "fast_copy.c"],
                   extra_compile_args=['-march=native', '-O3', '-Wall', '-Wextra', '-pedantic',
                                       '-Wshadow', '-Wpointer-arith', '-Wcast-qual',
                                       '-Wstrict-prototypes', '-Wmissing-prototypes',
                                       '-std=c++11', '-fopenmp'],
                   include_dirs=[np.get_include()] + ["pybind11/include"])

setup(name='FFHT',
      version='1.1',
      author='Ilya Razenshteyn, Ludwig Schmidt',
      author_email='falconn.lib@gmail.com',
      url='https://github.com/FALCONN-LIB/FFHT',
      description='Fast implementation of the Fast Hadamard Transform (FHT)',
      long_description=long_description,
      license='MIT',
      keywords='fast Fourier Hadamard transform butterfly',
      packages=find_packages(),
      include_package_data=True,
      ext_modules=[module])
