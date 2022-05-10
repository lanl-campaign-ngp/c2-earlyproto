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
BUILD_SHARED_LIBS                OFF
BUILD_TESTS                      OFF
CMAKE_BUILD_TYPE                 Release
CMAKE_EXE_LINKER_FLAGS           -static
CMAKE_INSTALL_PREFIX             /tmp/usr/local
CMAKE_PREFIX_PATH                /tmp/usr/local
CMAKE_SKIP_RPATH                 ON
fastbit_DIR                      /tmp/usr/local/share/cmake/fastbit
```

Type 'g' to generate build files and exit cmake. If 'g' is unavailable, type 'c' multiple times until 'g' appears. Once this is done, run `make` to build all targets. We want to statically link everything. For this make sure to set `BUILD_SHARED_LIBS` to `OFF`, `CMAKE_EXE_LINKER_FLAGS` to `-static`, and `CMAKE_SKIP_RPATH` to `ON`. Note that in the above example, we had assumed that FastBit was installed to `/tmp/usr/local` so `CMAKE_PREFIX_PATH` was set accordingly. With an appropriate `CMAKE_PREFIX_PATH`, `fastbit_DIR` will be populated automatically by cmake to indicate that it has successfully found a FastBit installation on the system.

```bash
make
```

Once `make` completes, the FastBit benchmark, `c2_index_bench`, will be available at `build/src/c2_index_bench`, which shall not have any dynamic dependencies.

# c2_index_bench Options

| Options             | Description                                                                                                  |
|---------------------|--------------------------------------------------------------------------------------------------------------|
| -n                  | Number of keys (doubles) to generate                                                                         |
| --skewed            | Whether the generated keys should form a uniform distribution or skewed                                      |
| --strategy          | Which fastbit binning strategy to use (0: simple, 1: linear, 2: equal-weight)                                |
| --nbins             | Number of bins to generate                                                                                   |
| --granule           | If the `precision reduction` binning strategy should be used instead of the one specified in `--strategy`    |
| --granule-precision | Number of precisions to preserve when `--granule` is ON (set to 1)                                           |
| -v                  | Verbose level for fastbit                                                                                    |

**Example 1**: `./c2_index_bench --granule=1 -n=10000000`.

This will use the `precision reduction` binning strategy (fastbit default) to build indexes on 10M keys generated using a uniform distribution. The precision level will be set at 2 (fastbit default) and the verbose level will be set at 0 (minimal prints). Eleven (11) queries will be run. Each targets 0%, 10%, 20%, ..., and 100% of keys.

**Example 2**: `./c2_index_bench --strategy=1 -n=10000000`.

This will use the `linear` binning strategy which tends to be less CPU intensive in index construction than the `precision reduction` used above. One disadvantage of using the `linear` strategy is that the bin boundaries may not align perfect with the queries, resulting in potentially additional data scans.

# c2_index_bench Results

Results for a `./c2_index_bench --granule=1 -n=10000000` run on different platforms. All runs used a single CPU core.

|                    | System A                        | System B       |
|--------------------|---------------------------------|----------------|
| CPU                | AMD EPYC 7502 32-Core Processor | ARM Cortex-A53 |
| Compiler           | GCC 8.4.1                       | GCC 10.2.1     |
| FastBit Index Time | 1.704s                          | 8.728s         |
| Query (0%)         | 0.000s                          | 0.000s         |
| Query (10%)        | 0.005s                          | 0.044s         |
| Query (20%)        | 0.009s                          | 0.080s         |
| Query (30%)        | 0.013s                          | 0.114s         |
| Query (40%)        | 0.017s                          | 0.149s         |
| Query (50%)        | 0.021s                          | 0.187s         |
| Query (60%)        | 0.016s                          | 0.150s         |
| Query (70%)        | 0.013s                          | 0.115s         |
| Query (80%)        | 0.009s                          | 0.080s         |
| Query (90%)        | 0.005s                          | 0.044s         |
| Query (100%)       | 0.000s                          | 0.000s         |

Results for a `./c2_index_bench --strategy=1 -n=10000000` run on different platforms. Again, all runs used a single CPU core.

|                    | System A                        | System B       |
|--------------------|---------------------------------|----------------|
| CPU                | AMD EPYC 7502 32-Core Processor | ARM Cortex-A53 |
| Compiler           | GCC 8.4.1                       | GCC 10.2.1     |
| FastBit Index Time | 0.799s                          | 4.741s         |
| Query (0%)         | 0.000s                          | 0.000s         |
| Query (10%)        | 0.005s                          | 0.043s         |
| Query (20%)        | 0.009s                          | 0.078s         |
| Query (30%)        | 0.013s                          | 0.113s         |
| Query (40%)        | 0.017s                          | 0.149s         |
| Query (50%)        | 0.021s                          | 0.186s         |
| Query (60%)        | 0.017s                          | 0.150s         |
| Query (70%)        | 0.013s                          | 0.115s         |
| Query (80%)        | 0.009s                          | 0.079s         |
| Query (90%)        | 0.005s                          | 0.043s         |
| Query (100%)       | 0.000s                          | 0.000s         |

Thanks!
