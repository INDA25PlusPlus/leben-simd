//
// Created by Leonard on 2026-01-20.
//

#pragma once

#include "clock.h"


float average_secs(timespec_t *times, unsigned size, unsigned batch_size);

float stddev_secs(timespec_t *times, unsigned size, float average, unsigned batch_size);
