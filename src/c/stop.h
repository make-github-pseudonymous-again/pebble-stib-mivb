#pragma once
#include <pebble.h>

typedef struct Stop {
	uint32_t stop_id;
	const char* stop_name;
	const ds_DynamicArray realtimes;
} Stop;

Stop* Stop_create(
	const uint32_t stop_id,
	const char *stop_name
);

void Stop_free(const Stop *stop);
