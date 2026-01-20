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

target_compile_options(leben_simd PRIVATE -mfma -msse4.1 -mavx2)

target_link_libraries(leben_simd m)
