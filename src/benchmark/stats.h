//
// Created by Leonard on 2026-01-20.
//

#pragma once

#include "clock.h"


float average_ns(timespec_t *times, unsigned batch_count, unsigned batch_size);

float stddev_ns(timespec_t *times, unsigned batch_count, float average, unsigned batch_size);
