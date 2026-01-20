//
// Created by Leonard on 2026-01-16.
//

#include <xmmintrin.h>
#include <stdbool.h>

#include "mandelbrot.h"
#include "config.h"
#include "math_util.h"


/**
 * c points at {c_real, c_imag}
 * z points at {z_real, z_imag}
 */
void mandelbrot_iter(__m256 const *c, __m256 *z) {
    __m256 const two = _mm256_set1_ps(2.f);

    // z1 = z0^2 + c
    // = (zre + zim i)^2 + cre + cim i
    // = (zre^2 - zim + cre) + (2 zre im + cim) i

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
 * c points at {c_real, c_imag}
 * c and depth must be aligned as __m256
 */
void mandelbrot(float const *c, unsigned *depth) {
    __m256 const max_abs_v = _mm256_set1_ps(max_abs);

    __m256 c_v[2];
    c_v[0] = _mm256_load_ps(c);
    c_v[1] = _mm256_load_ps(&c[8]);

    __m256 z[2];
    z[0] = c_v[0];
    z[1] = c_v[1];

    __m256i depth_v = _mm256_load_si256((__m256i *) depth);

    for (int current_depth = 0; current_depth < max_depth; current_depth++) {
        mandelbrot_iter(c_v, z);

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
