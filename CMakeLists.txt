cmake_minimum_required(VERSION 4.1)
project(leben_simd C)

set(CMAKE_C_STANDARD 11)

add_executable(leben_simd src/main.c
        src/mandelbrot.h
        src/mandelbrot.c
        src/config.h
        src/math_util.h
        src/math_util.c
)

# fma: fma instructions (fused mult-add)
# sse4.1: SSE instruction set extension
# avx2: AVX instruction set extension
target_compile_options(leben_simd PRIVATE -mfma -msse4.1 -mavx2)

# std math library
target_link_libraries(leben_simd m)
