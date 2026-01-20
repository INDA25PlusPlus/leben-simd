# leben-simd

*'The only thing I hate more than object oriented programming is slow execution time'* <br>
*\- Benoit Mandelbrot, probably*

Channels the power of SIMD (single instruction, multiple data) to produce blazingly fast 🔥 renders of the Mandelbrot set.

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
