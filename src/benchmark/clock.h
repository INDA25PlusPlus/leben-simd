//
// Created by Leonard on 2026-01-20.
//

#pragma once

#include <time.h>


typedef struct timespec timespec_t;

void start_timer();

timespec_t stop_timer();
