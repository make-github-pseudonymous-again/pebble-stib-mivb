#pragma once
#include <pebble.h>
#include "ds/dynamicarray.h"

typedef struct Stop {
	uint32_t id;
	const char* name;
	bool error;
	const char *message;
	const ds_DynamicArray realtime;
} Stop;

Stop* Stop_create(
	const uint32_t id,
	const char *name,
	bool error,
	const char* message
);

void Stop_destroy(const Stop *stop);
