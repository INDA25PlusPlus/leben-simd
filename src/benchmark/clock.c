//
// Created by Leonard on 2026-01-20.
//

#include "clock.h"


timespec_t static timer;

// measures actual cpu usage
clockid_t const static clock_id = CLOCK_PROCESS_CPUTIME_ID;

timespec_t diff(timespec_t start, timespec_t end) {
    // from https://stackoverflow.com/a/6749766
    timespec_t temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

void start_timer() {
    clock_gettime(clock_id, &timer);
}

timespec_t stop_timer() {
    timespec_t now;
    clock_gettime(clock_id, &now);
    return diff(timer, now);
}
