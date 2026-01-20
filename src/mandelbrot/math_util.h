//
// Created by Leonard on 2026-01-18.
//

#pragma once

#include <immintrin.h>

__m256 m256_abs(__m256 x);

__m256i m256i_blend(__m256i a, __m256i b, __m256i mask);

float first(__m256 a);
