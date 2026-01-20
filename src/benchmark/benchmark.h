//
// Created by Leonard on 2026-01-20.
//

#pragma once

#include <stdbool.h>

#include "clock.h"


typedef struct benchmark_params {
    unsigned batch_count;
    unsigned batch_size;
    unsigned x_res;
    unsigned y_res;
    float x_center;
    float y_center;
    float view_height;
    unsigned max_iterations;
} benchmark_params_t;

typedef struct benchmark_results {
    timespec_t *sisd_times;
    timespec_t *simd_times;
} benchmark_results_t;

void destroy_benchmark_results(benchmark_results_t *results);

benchmark_results_t benchmark(benchmark_params_t params, bool print_progress);
