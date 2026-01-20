//
// Created by Leonard on 2026-01-19.
//

#include "math_util.h"


__m256 m256_abs(__m256 x) {
    // structure of 32-bit float:
    // seee eeee emmm mmmm mmmm mmmm mmmm mmmm
    // s = sign, e = exponent, m = mantissa
    // sign 0 = positive, so we set the first bit
    // to 0 by and-ing with 0x7fffffff
    __m256 const abs_mask = _mm256_castsi256_ps(
        _mm256_set1_epi32(0x7fffffff));
    return _mm256_and_ps(x, abs_mask);
}

__m256i m256i_blend(__m256i a, __m256i b, __m256i mask) {
    // (mask & a) | (~mask & b)
    return _mm256_or_si256(
        _mm256_and_si256(mask, a),
        _mm256_andnot_si256(mask, b));
}
