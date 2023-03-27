# PyWavpack (Under development)
## Depenedencies

PyWavpack depends on [nanobind](https://github.com/wjakob/nanobind) to build a Python extension for [Wavpack](https://github.com/dbry/WavPack), therefore following the depenedencies of nanobind.

- Python3.8+ or PyPy 7.3.10+
- CMake 3.15+
- A C++17 compiler: Clang 7+, GCC 8+, and MSVC2019+ are officially supported. Others (MinGW, Intel, NVIDIA, ..) may work as well but will not receive support.
- Wavpack 5.6.0 Release


Note that Wavpack is originally created by C-language, so the depenedency on C++ comes from this PyWavpack.

## Installing


```sh
git clone --recursive https://github.com/kamo-naoyuki/pywavpack.git
pip install pywavpack/
```

## Usage (Under development)

### Reading API

```python
import pywavpack.pywavpack_ext as E
# Always returned as int32 2d-array
numpy_ndarray, sampling_rate = E.read("test.wv")
```

### Writing API

Not yet


## Miscellaneous

Wavpack is a completely open source project under BSD-3-Clause license. Libsndfile is also under development to support Wavpack: https://github.com/libsndfile/libsndfile/issues/15 and https://github.com/libsndfile/libsndfile/pull/661. It's ideally to incorporate Wavpack in libsndfile, because it's useful to be aware of only an entrance library for the viewpoint of end users. If the wavpack supporting is completed in libsndfile, I might close this project.
