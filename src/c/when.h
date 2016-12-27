#pragma once
#include <pebble.h>

static const int WHEN_GONE_THRESHOLD = -600;
static const size_t WHEN_BUFFER_SIZE = 4;

// each string below must have size <= MINUTES_BUFFER_SIZE - 1
static const char *WHEN_IN_MORE_THAN_100_MINUTES = ">99";
static const char *WHEN_GONE = ":(";
static const char *WHEN_NOW = "0";


static GColor when ( char *buffer, const time_t now, const time_t expected_arrival ) ;
