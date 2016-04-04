#!/bin/bash

CMAKE_FLAGS="-DCMAKE_INSTALL_PREFIX=$PREFIX -DBUILD_TESTING=OFF"
CMAKE_FLAGS+=" -DREADDY_BUILD_SHARED_COMBINED:BOOL=ON"
CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Release"

export PYTHON_INCLUDE_DIR=`python -c "from __future__ import print_function; import distutils.sysconfig; print(distutils.sysconfig.get_python_inc(True))"`

# attempt to feed the right python library to FindPythonLibs
lib_path=""
if [ `uname` == Darwin ]; then
    if [ $PY3K -eq 1 ]; then
        lib_path="$PREFIX/lib/libpython${PY_VER}m.dylib"
    else
        lib_path="$PREFIX/lib/libpython${PY_VER}.dylib"
    fi
fi
if [ `uname` == Linux ]; then
    if [ $PY3K -eq 1 ]; then
        lib_path="$PREFIX/lib/libpython${PY_VER}m.so"
    else
        lib_path="$PREFIX/lib/libpython${PY_VER}.so"
    fi
fi
CMAKE_FLAGS+=" -DPYTHON_LIBRARY:FILEPATH=${lib_path}"

# cant reliably determine cpu count in a docker container,
# therefore fix this value.
if [ "$TRAVIS" == "true" ]; then CPU_COUNT=2; fi

mkdir build
cd build
echo "calling cmake with flags ${CMAKE_FLAGS}"
cmake .. ${CMAKE_FLAGS}
make -j${CPU_COUNT}
make -j${CPU_COUNT} install &> /dev/null