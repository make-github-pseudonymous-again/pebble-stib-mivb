#pragma once
#include <pebble.h>

typedef struct Minutes {
	const GColor color ;
	const int32_t minutes ;
} Minutes ;

static Minutes when ( time_t now, time_t expected_arrival ) ;