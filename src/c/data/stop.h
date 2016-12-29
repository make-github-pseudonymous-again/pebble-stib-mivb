#pragma once
#include <pebble.h>
#include "../ds/dynamicarray.h"

typedef struct Stop {
	uint32_t id;
	const char* name;
	uint32_t error;
	const char *message;
	ds_DynamicArray realtime;
} Stop;

Stop* Stop_create(
	const uint32_t id,
	const char *name,
	const uint32_t error,
	const char* message
);

void Stop_destroy(Stop *stop);
