//
// Created by Leonard on 2026-01-20.
//

#include "stats.h"

#include <math.h>


float timespec_to_ns(timespec_t time);

float average_ns(timespec_t *times, unsigned batch_count, unsigned batch_size) {
    float sum = 0;
    float factor = 1.f / batch_size;
    for (unsigned i = 0; i < batch_count; i++) {
        sum += timespec_to_ns(times[i]) * factor;
    }
    return sum / batch_count;
}

float stddev_ns(timespec_t *times, unsigned batch_count, float average, unsigned batch_size) {
    float sum = 0;
    float factor = 1.f / batch_size;
    for (unsigned i = 0; i < batch_count; i++) {
        float diff = average - timespec_to_ns(times[i]) * factor;
        sum += batch_size * diff * diff;
    }
    return sqrtf(sum / ((float) (batch_count * batch_size) - 1));
}

float timespec_to_ns(timespec_t time) {
    return (float) time.tv_sec * 1000000000.f + (float) time.tv_nsec;
}
