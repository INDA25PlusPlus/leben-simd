cmake_minimum_required(VERSION 4.1)
project(leben_simd C)

set(CMAKE_C_STANDARD 11)

add_executable(leben_simd src/main.c
        src/mandelbrot/mandelbrot.c
        src/mandelbrot/mandelbrot.h
        src/config.h
        src/mandelbrot/math_util.c
        src/mandelbrot/math_util.h
        src/img/stb_image_write.c
        src/img/stb_image_write.h
        src/benchmark/clock.c
        src/benchmark/clock.h
        src/benchmark/benchmark.c
        src/benchmark/benchmark.h
)

# fma: fma instructions (fused mult-add)
# sse4.1: SSE instruction set extension
# avx2: AVX instruction set extension
target_compile_options(leben_simd PRIVATE -mfma -msse4.1 -mavx2)

# std math library
target_link_libraries(leben_simd m)
