#pragma once
#include <pebble.h>

const int WHEN_GONE_THRESHOLD = -600;
#define WHEN_BUFFER_SIZE 4

// each string below must have size <= MINUTES_BUFFER_SIZE - 1
const char *WHEN_IN_MORE_THAN_100_MINUTES = ">99";
const char *WHEN_GONE = ":(";
const char *WHEN_NOW = "0";

GColor when ( char *buffer, const time_t now, const time_t expected_arrival ) ;