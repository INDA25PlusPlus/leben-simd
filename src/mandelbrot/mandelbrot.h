//
// Created by Leonard on 2026-01-19.
//

#pragma once


void mandelbrot(float const *c, unsigned *depth, unsigned max_iterations);

typedef struct mandelbrot_calc {
    float *c_values;
    unsigned *depth;
    unsigned const x_res;
    unsigned const y_res;
} mandelbrot_calc_t;

mandelbrot_calc_t make_mandelbrot_calc(unsigned x_res, unsigned y_res);

void init_mandelbrot_calc(mandelbrot_calc_t const *calc, float x_center, float y_center, float view_height, unsigned max_iterations);

void run_mandelbrot_calc(mandelbrot_calc_t const *calc, unsigned max_iterations);

float get_mandelbrot_intensity(mandelbrot_calc_t const *calc, unsigned x, unsigned y, unsigned max_iterations);

void destroy_mandelbrot_calc(mandelbrot_calc_t *calc);
