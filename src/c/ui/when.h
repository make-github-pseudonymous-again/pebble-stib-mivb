#pragma once
#include <pebble.h>

#define WHEN_BUFFER_SIZE 4

extern const int WHEN_GONE_THRESHOLD;
extern const char *WHEN_IN_MORE_THAN_100_MINUTES;
extern const char *WHEN_GONE;
extern const char *WHEN_NOW;

GColor when ( char *buffer, const time_t now, const time_t expected_arrival ) ;