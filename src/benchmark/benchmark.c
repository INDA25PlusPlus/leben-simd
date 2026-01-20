//
// Created by Leonard on 2026-01-20.
//

#include "benchmark.h"

#include <stdlib.h>

#include "../mandelbrot/mandelbrot.h"


void time_mandelbrot(
    unsigned batch_count, unsigned batch_size, mandelbrot_calc_t *calc,
    unsigned max_iterations, timespec_t *batch_times);

void time_mandelbrot_simd(
    unsigned batch_count, unsigned batch_size, mandelbrot_calc_t *calc,
    unsigned max_iterations, timespec_t *batch_times);

void destroy_benchmark_results(benchmark_results_t *results) {
    free(results->sisd_times);
    results->sisd_times = NULL;
    free(results->simd_times);
    results->simd_times = NULL;
}

benchmark_results_t benchmark(benchmark_params_t params) {
    mandelbrot_calc_t calc = make_mandelbrot_calc(params.x_res, params.y_res);

    timespec_t *sisd_times = malloc(sizeof(timespec_t) * params.batch_count);
    timespec_t *simd_times = malloc(sizeof(timespec_t) * params.batch_count);

    init_mandelbrot_calc(
        &calc, params.x_center, params.y_center, params.view_height,
        params.max_iterations, 1.f);
    time_mandelbrot(params.batch_count, params.batch_size, &calc,
        params.max_iterations, sisd_times);

    init_mandelbrot_calc(
        &calc, params.x_center, params.y_center, params.view_height,
        params.max_iterations, 1.f);
    time_mandelbrot_simd(params.batch_count, params.batch_size, &calc,
        params.max_iterations, simd_times);

    return (benchmark_results_t) {
        sisd_times,
        simd_times
    };
}

/**
 * Size of batch_times is batch_count
 */
void time_mandelbrot(
    unsigned batch_count, unsigned batch_size, mandelbrot_calc_t *calc,
    unsigned max_iterations, timespec_t *batch_times)
{
    for (unsigned i = 0; i < batch_count; i++) {
        // warmup
        for (unsigned j = 0; j < batch_size; j++) {
            run_mandelbrot_calc(calc, max_iterations);
        }
        start_timer();
        for (unsigned j = 0; j < batch_size; j++) {
            run_mandelbrot_calc(calc, max_iterations);
        }
        batch_times[i] = stop_timer();
    }
}

/**
 * Size of batch_times is batch_count
 */
void time_mandelbrot_simd(
    unsigned batch_count, unsigned batch_size, mandelbrot_calc_t *calc,
    unsigned max_iterations, timespec_t *batch_times)
{
    for (unsigned i = 0; i < batch_count; i++) {
        // warmup
        for (unsigned j = 0; j < batch_size; j++) {
            run_mandelbrot_calc_simd(calc, max_iterations);
        }
        start_timer();
        for (unsigned j = 0; j < batch_size; j++) {
            run_mandelbrot_calc_simd(calc, max_iterations);
        }
        batch_times[i] = stop_timer();
    }
}
