#pragma once
#include <pebble.h>

typedef struct Realtime {
	uint32_t stop_id;
	const char* line_number;
	const char* destination_name;
	uint32_t foreground_color;
	uint32_t background_color;
	uint32_t utc;
} Realtime;

Realtime* Realtime_create(
	const uint32_t stop_id,
	const char *line_number,
	const char *destination_name,
	const uint32_t foreground_color,
	const uint32_t background_color,
	const uint32_t utc
);

void Realtime_destroy(Realtime *realtime);

status_t Realtime_persist_write(uint32_t *key, const Realtime *realtime);
Realtime* Realtime_persist_read(uint32_t *key, const uint32_t stop_id);
void Realtime_persist_skip(uint32_t *key);