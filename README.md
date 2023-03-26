# PyWavpack (Under development)
## Depenedencies

PyWavpack depends on [nanobind](https://github.com/wjakob/nanobind) to build a Python extension for [Wavpack](https://github.com/dbry/WavPack), therefore following the depenedencies of nanobind.

- Python3.8+ or PyPy 7.3.10+
- CMake 3.15+
- A C++17 compiler: Clang 7+, GCC 8+, and MSVC2019+ are officially supported. Others (MinGW, Intel, NVIDIA, ..) may work as well but will not receive support.

## Installing


```sh
git clone --recursive https://github.com/kamo-naoyuki/pywavpack.git
pip install pywavpack/
```

## Usage (Under development)

### Reading API

```python
import pywavpack.pywavpack_ext as E
array, rate = E.read("test.wv")
```

### Writing API

Not yet
