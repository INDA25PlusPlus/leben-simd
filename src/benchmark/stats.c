//
// Created by Leonard on 2026-01-20.
//

#include "stats.h"

#include <math.h>


float timespec_to_secs(timespec_t time);

float average_secs(timespec_t *times, unsigned size, unsigned batch_size) {
    float sum = 0;
    float factor = 1.f / batch_size;
    for (unsigned i = 0; i < size; i++) {
        sum += timespec_to_secs(times[i]) * factor;
    }
    return sum / size;
}

float stddev_secs(timespec_t *times, unsigned size, float average, unsigned batch_size) {
    float sum = 0;
    float factor = 1.f / batch_size;
    for (unsigned i = 0; i < size; i++) {
        float diff = average - timespec_to_secs(times[i]) * factor;
        sum += diff * diff;
    }
    return sqrtf(sum / ((float) size - 1));
}

float timespec_to_secs(timespec_t time) {
    return (float) time.tv_sec + (float) time.tv_nsec / 1000000000.f;
}
