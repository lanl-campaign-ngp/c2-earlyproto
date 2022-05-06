**Early experimental code for LANL's next-gen campaign storage**

[![License](https://img.shields.io/badge/license-New%20BSD-blue.svg)](LICENSE.txt)

C2 early-proto
================

```
XX              XXXXX XXX         XX XX           XX       XX XX XXX         XXX
XX             XXX XX XXXX        XX XX           XX       XX XX    XX     XX   XX
XX            XX   XX XX XX       XX XX           XX       XX XX      XX XX       XX
XX           XX    XX XX  XX      XX XX           XX       XX XX      XX XX       XX
XX          XX     XX XX   XX     XX XX           XX XXXXX XX XX      XX XX       XX
XX         XX      XX XX    XX    XX XX           XX       XX XX     XX  XX
XX        XX       XX XX     XX   XX XX           XX       XX XX    XX   XX
XX       XX XX XX XXX XX      XX  XX XX           XX XXXXX XX XX XXX     XX       XX
XX      XX         XX XX       XX XX XX           XX       XX XX         XX       XX
XX     XX          XX XX        X XX XX           XX       XX XX         XX       XX
XX    XX           XX XX          XX XX           XX       XX XX          XX     XX
XXXX XX            XX XX          XX XXXXXXXXXX   XX       XX XX            XXXXXX
```

This is the early experimental code for LANL's next-gen campaign storage named C2. Included with the code is a small FastBit benchmark that one can use to measure FastBit index construction and query performance on different computing platforms and systems. C2 is developed under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. See the accompanying LICENSE.txt for further information.

# Platform

We focus on Linux at the moment.

# Software requirements

Compiling C2 currently requires g++, cmake, and make. The included FastBit benchmark requires an installation of FastBit. On Ubuntu, one may use the following commands to prepare the programming environment for C2. To build and install FastBit, see our companion codebase [c2-fastbit](https://github.com/lanl-future-campaign/c2-fastbit). When configuring FastBit, make sure to set `BUILD_SHARED_LIBS` to `OFF` so that a `libfastbit.a` is built instead of a `libfastbit.so`. We will configure C2 to statically link with this `libfastbit.a`.

```bash
sudo apt-get install g++ make cmake cmake-curses-gui
```

For Ubuntu 20.04.4, this will install g++ 9.4.0, cmake 3.16.3, and make 4.2.1.

# Building

After all software requirements are installed (including FastBit), use the following to configure and build c2.

```bash
git clone https://github.com/lanl-future-campaign/c2-earlyproto.git
cd c2-earlyproto
mkdir build
cd build
ccmake -DCMAKE_PREFIX_PATH=<c2_fastbit_installdir> ..
```

Type 'c' multiple times and choose the following set of options:

```bash
BUILD_SHARED_LIBS                *OFF
BUILD_TESTS                      *OFF
CMAKE_BUILD_TYPE                 *Release
CMAKE_INSTALL_PREFIX             */tmp/usr/local
CMAKE_PREFIX_PATH                */tmp/usr/local
fastbit_DIR                      */tmp/usr/local/share/cmake/fastbit
```

Type 'g' to generate build files and exit cmake. If 'g' is not available, type 'c' multiple times until 'g' appears. Once this is done, run `make` to build all targets.

```bash
make
```

Once `make` completes, the FastBit benchmark, `c2_index_bench`, will be available at `build/src/c2_index_bench`.
