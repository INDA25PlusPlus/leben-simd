# leben-simd

*'The only thing I love more than complex numbers is fast execution time'* <br>
*\- Benoît Mandelbrot, probably*

Channels the power of SIMD (single instruction, multiple data) to produce blazingly fast 🔥 renders of the Mandelbrot set. See below for comparisons:

## Comparisons

The following benchmarks were done on an Intel Core i7-8665U CPU. Benchmarks were done in batches, with each batch preceded by a warmup batch in order to minimize predictive branching and caching disturbances.

Measurements "3.0 ± 1.0 ms" indicate average and standard deviation and are rounded to 4 significant digits. Note that the measured time only includes time spent performing calculations, and not initializing data, outputting images etc.

A complete log can be found at [readme/benchmark.log](readme/benchmark.log).

### Standard full 1920x1080 picture, 50 iterations

<img src="readme/standard.png" width="50%">

`build/leben_simd --bench 100 100 1920 1080 -.5 0 3 50`

(100 batches of 100 runs per build and algorithm type)

| Build type          | SISD             | SIMD             | Times faster |
|---------------------|------------------|------------------|--------------|
| Debug (O0)          | 137.4 ± 1.589 ms | 72.75 ± 3.272 ms | 4.95x        |
| RelWithDebInfo (O2) | 62.85 ± .5334 ms | 14.97 ± .3833 ms | 4.20x        |
| Release (O3)        | 62.83 ± 2.231 ms | 14.16 ± .2194 ms | 4.44x        |

### Zoomed-in mid-iterations 1920x1080 picture, 400 iterations

<img src="readme/zoomed-1.png" width="50%">

`build/leben_simd --bench 10 100 1920 1080 -1.3896051 -.0140649 .0001 400`

(10 batches of 100 runs per build and algorithm type)

| Build type          | SISD             | SIMD             | Times faster |
|---------------------|------------------|------------------|--------------|
| Debug (O0)          | 2830. ± 18.62 ms | 1591. ± 23.80 ms | 1.78x        |
| RelWithDebInfo (O2) | 1465. ± 17.04 ms | 297.6 ± 1.543 ms | 4.93x        |
| Release (O3)        | 1462. ± 1.535 ms | 290.9 ± 1.003 ms | 5.03x        |

### Zoomed-in high-iterations 1920x1080 picture, 6400 iterations

<img src="readme/zoomed-2.png" width="50%">

`build/leben_simd --bench 4 50 1920 1080 .27605 -.00741 .0001 6400`

(4 batches of 50 runs per build and algorithm type)

| Build type          | SISD             | SIMD             | Times faster |
|---------------------|------------------|------------------|--------------|
| Debug (O0)          | 3662. ± 130.1 ms | 941.8 ± 73.91 ms | 3.89x        |
| RelWithDebInfo (O2) | 4072. ± 4.849 ms | 957.4 ± 3.926 ms | 4.26x        |
| Release (O3)        | 4089. ± 6.249 ms | 943.8 ± 1.847 ms | 4.33x        |

### Discussion

Overall, we see a ~4-5x improvement for SIMD over SISD for optimized builds, for both low-iteration and high-iteration renders. Compared to the theoretical 8x improvement in parallelization (using 256-bit SIMD instruction with 32-bit floats), this is a pretty good result. This improvement does not seem to correlate (or at least not very strongly) with the iteration count. This indicates that the CPU time is in large majority spent on the main iteration loop, and not on other parts of the routine which are unaffected by the SIMD optimization. Additionally, the time differences between O2 and O3 level runs are statistically insignificant, suggesting that optimizations past O2 do not affect the high-runtime main iterative algorithm.

## Usage

### Building

Requires a CPU with the Intel SSE and AVX instruction set extensions to run the binary.

Build and configure using CMake:

`cmake -B build/`

`cmake --build build/`

### Running

Syntax: `exe [<x_res> <y_res> <x_pos> <y_pos> <view_height> <max_iterations>] [<out_path> [-f]]`

Generates an image of the Mandelbrot set using the given parameters, and outputs it to the console using ASCII art.

If `<out_path>` is specified, the image is instead stored to file. Add `-f` to overwrite existing files. 

### Benchmarking

Syntax: `exe --bench <batch_count> <batch_size> <x_res> <y_res> <x_pos> <y_pos> <view_height> <max_iterations>`

Runs a benchmark of the implementation using both SISD (single instruction, single data) and SIMD (single instruction, multiple data) algorithms. Prints the average and standard deviation of the run time for SISD and SIMD respectively.

### Benchmarking script

Use the benchmarking script to run multiple benchmarks using different optimization levels:

`bash benchmark_suite.sh`

Builds and runs the project with the following build types:
- `Debug`, corresponding to `O0` (no optimizations)
- `RelWithDebInfo`, corresponding to `O2` (moderate optimizations)
- `Release`, corresponding to `O3` (full optimizations)

The script output is saved to a log file.
