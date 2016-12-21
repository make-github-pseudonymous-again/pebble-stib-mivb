#pragma once
#include <pebble.h>

typedef struct Realtime {
	uint32_t stop_id;
	char* stop_name;
	char* line_name;
	char* destination_name;
	uint32_t foreground_color;
	uint32_t background_color;
	uint32_t utc;
} Realtime;

Realtime* Realtime_create(
	const uint32_t stop_id,
	const char *stop_name,
	const char *line_name,
	const char *destination_name,
	const uint32_t foreground_color,
	const uint32_t background_color,
	const uint32_t utc
);