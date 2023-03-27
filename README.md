# PyWavPack (Under development)
## Depenedencies

PyWavPack depends on [nanobind](https://github.com/wjakob/nanobind) to build a Python extension for [WavPack](https://github.com/dbry/WavPack), therefore following the depenedencies of nanobind.

- Python3.8+ or PyPy 7.3.10+
- CMake 3.15+
- A C++17 compiler: Clang 7+, GCC 8+, and MSVC2019+ are officially supported. Others (MinGW, Intel, NVIDIA, ..) may work as well but will not receive support.
- WavPack 5.6.0 Release


Note that WavPack is originally created by C-language, so the depenedency on C++ comes from this PyWavPack.

## Installing


```sh
git clone --recursive https://github.com/kamo-naoyuki/pywavpack.git
pip install pywavpack/
```

## Usage (Under development)

### Reading API

```python
import pywavpack.pywavpack_ext as E
# Always returned as int32 2d-array with the shape of (frames, channels)
numpy_ndarray, sampling_rate = E.read("test.wv")
```

### Writing API

Not yet


## Miscellaneous

WavPack is a completely open source project under BSD-3-Clause license. Libsndfile is also under development to support WavPack: https://github.com/libsndfile/libsndfile/issues/15 and https://github.com/libsndfile/libsndfile/pull/661. It's ideally to incorporate WavPack in libsndfile, because it's useful to be aware of only an entrance library for the viewpoint of end users. If the WavPack supporting is completed in libsndfile, I might close this project.
