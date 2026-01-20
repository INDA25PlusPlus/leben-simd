//
// Created by Leonard on 2026-01-16.
//

#include <xmmintrin.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdio.h>

#include "mandelbrot.h"

#include <math.h>

#include "../config.h"
#include "math_util.h"


/**
 * c[0..8] = c_real, c[8..16] = c_imag
 */
void mandelbrot(float const *c, unsigned *depth, unsigned max_iterations) {
    for (unsigned i = 0; i < 8; i++) {
        float const cre = c[0 + i];
        float const cim = c[8 + i];
        float zre = cre;
        float zim = cim;
        for (int current_depth = 0; current_depth < max_iterations; current_depth++) {
            // z1 = z0^2 + c
            // = (zre + zim i)^2 + cre + cim i
            // = (zre^2 - zim^2 + cre) + (2 zre zim + cim) i
            float zre_temp = fmaf(zre, zre, -fmaf(zim, zim, -cre));
            zim = 2 * zre * zim + cim;
            zre = zre_temp;

            // approximate |z|
            float z_abs = fmaxf(fabsf(zre), fabsf(zim));

            if (z_abs > max_abs) {
                break;
            }
            depth[i] = current_depth;
        }
    }
}

/**
 * c points at {c_real, c_imag}
 * z points at {z_real, z_imag}
 */
void mandelbrot_simd_iter(__m256 const *c, __m256 *z) {
    __m256 const two = _mm256_set1_ps(2.f);

    // z1 = z0^2 + c
    // = (zre + zim i)^2 + cre + cim i
    // = (zre^2 - zim^2 + cre) + (2 zre zim + cim) i

    __m256 zre = z[0];
    __m256 zim = z[1];
    __m256 cre = c[0];
    __m256 cim = c[1];

    // real part:
    // zre^2 + -zim^2 + cre
    z[0] = _mm256_fmadd_ps(zre, zre, _mm256_fnmadd_ps(zim, zim, cre));

    // imaginary part:
    // 2 * zre * zim + cim
    z[1] = _mm256_fmadd_ps(two, _mm256_mul_ps(zre, zim), cim);
}

/**
 * c[0..8] = c_real, c[8..16] = c_imag
 * c and depth must be aligned as __m256
 */
void mandelbrot_simd(float const *c, unsigned *depth, unsigned max_iterations) {
    __m256 const max_abs_v = _mm256_set1_ps(max_abs);

    __m256 c_v[2];
    c_v[0] = _mm256_load_ps(c);
    c_v[1] = _mm256_load_ps(&c[8]);

    __m256 z[2];
    z[0] = c_v[0];
    z[1] = c_v[1];

    __m256i depth_v = _mm256_load_si256((__m256i *) depth);

    for (int current_depth = 0; current_depth < max_iterations; current_depth++) {
        mandelbrot_simd_iter(c_v, z);

        __m256 zre_abs = m256_abs(z[0]);
        __m256 zim_abs = m256_abs(z[1]);
        // approximate |z|
        __m256 z_abs = _mm256_max_ps(zre_abs, zim_abs);

        // 0x00000000 when z_abs > max_abs
        // 0xffffffff when z_abs <= max_abs
        __m256 not_diverged_mask = _mm256_cmp_ps(z_abs, max_abs_v, _CMP_LE_OS);
        __m256i not_diverged_mask_i = _mm256_castps_si256(not_diverged_mask);

        __m256i current_depth_v = _mm256_set1_epi32(current_depth);

        // update to current depth where z_abs <= max_abs
        depth_v = m256i_blend(current_depth_v, depth_v, not_diverged_mask_i);

        // true if all are zero
        bool all_diverged = _mm256_testz_ps(not_diverged_mask, not_diverged_mask);
        if (all_diverged)
            break;
    }

    _mm256_store_si256((__m256i *) depth, depth_v);
}

mandelbrot_calc_t make_mandelbrot_calc(unsigned x_res, unsigned y_res) {
    if (x_res % 8 != 0) {
        printf("x_res must be multiple of 8!");
        exit(EXIT_FAILURE);
    }

    unsigned size = x_res * y_res;

    // NOTE: length is 2 * size
    float *c_values = aligned_alloc(alignof(__m256), sizeof(float) * 2 * size);
    unsigned *depth = aligned_alloc(alignof(__m256i), sizeof(int) * size);

    if (c_values == NULL || depth == NULL) {
        printf("mandelbrot_calc allocation failed");
        exit(EXIT_FAILURE);
    }

    mandelbrot_calc_t calc = {
        c_values,
        depth,
        x_res,
        y_res
    };
    return calc;
}

void init_mandelbrot_calc(mandelbrot_calc_t const *calc, float x_center, float y_center, float view_height, unsigned max_iterations, float pixel_aspect_ratio) {
    float scale = view_height / (float) calc->y_res;
    float horizontal_scale = scale / pixel_aspect_ratio;
    float view_width = horizontal_scale * (float) calc->x_res;

    for (unsigned y = 0; y < calc->y_res; y++) {
        float cim = (float) y * scale - .5f * view_height + y_center;

        for (unsigned x = 0; x < calc->x_res; x += 8) {
            unsigned index = y * calc->x_res + x;

            for (unsigned xs = 0; xs < 8; xs++) {
                float cre = (float) (x + xs) * horizontal_scale - .5f * view_width + x_center;

                calc->c_values[2 * index + xs] = cre;
                calc->c_values[2 * index + 8 + xs] = cim;
                calc->depth[index + xs] = max_iterations;
            }
        }
    }
}

void run_mandelbrot_calc(mandelbrot_calc_t const *calc, unsigned max_iterations) {
    for (unsigned y = 0; y < calc->y_res; y++) {
        for (unsigned x = 0; x < calc->x_res; x += 8) {
            unsigned index = y * calc->x_res + x;
            mandelbrot(&calc->c_values[2 * index], &calc->depth[index], max_iterations);
        }
    }
}

void run_mandelbrot_calc_simd(mandelbrot_calc_t const *calc, unsigned max_iterations) {
    for (unsigned y = 0; y < calc->y_res; y++) {
        for (unsigned x = 0; x < calc->x_res; x += 8) {
            unsigned index = y * calc->x_res + x;
            mandelbrot_simd(&calc->c_values[2 * index], &calc->depth[index], max_iterations);
        }
    }
}

float get_mandelbrot_intensity(mandelbrot_calc_t const *calc, unsigned x, unsigned y, unsigned max_iterations) {
    unsigned index = y * calc->x_res + x;
    return (float) calc->depth[index] / (float) max_iterations;
}

void destroy_mandelbrot_calc(mandelbrot_calc_t *calc) {
    free(calc->c_values);
    calc->c_values = NULL;
    free(calc->depth);
    calc->depth = NULL;
}
