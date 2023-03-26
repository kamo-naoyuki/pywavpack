import sys, re, os

try:
    from skbuild import setup
    import nanobind
except ImportError:
    print("The preferred way to invoke 'setup.py' is via pip, as in 'pip "
          "install .'. If you wish to run the setup script directly, you must "
          "first install the build dependencies listed in pyproject.toml!",
          file=sys.stderr)
    raise

setup(
    name="pywavpack",
    version="0.0.0",
    author="Naoyuki Kamo",
    author_email="naoyuki.kamo829@gmail.com",
    description="An example minimal project that compiles bindings using nanobind and scikit-build",
    url="https://github.com/kamo-naoyuki/pywavpack",
    license="BSD",
    packages=['pywavpack'],
    package_dir={'': 'src'},
    cmake_install_dir="src/pywavpack",
    include_package_data=True,
    python_requires=">=3.8"
)
