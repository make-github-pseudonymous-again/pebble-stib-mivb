#include <pebble.h>
#include "realtime.h"
#include "std/strdup.h"

Realtime* Realtime_create(
	const uint32_t stop_id,
	const char *stop_name,
	const char *line_number,
	const char *destination_name,
	const uint32_t foreground_color,
	const uint32_t background_color,
	const uint32_t utc	
) {
	Realtime *realtime = malloc(sizeof(Realtime));
	
	if (realtime == NULL) return NULL;
	
	const char *stop_name_copy = strdup(stop_name);
	const char *line_number_copy = strdup(line_number);
	const char *destination_name_copy = strdup(destination_name);

	*realtime = (Realtime) {
		.stop_id = stop_id,
		.stop_name = stop_name_copy,
		.line_number = line_number_copy,
		.destination_name = destination_name_copy,
		.foreground_color = foreground_color,
		.background_color = background_color,
		.utc = utc
	};

	return realtime;
}

void Realtime_free(const Realtime *realtime) {
	free((void*)realtime->stop_name);
	free((void*)realtime->line_number);
	free((void*)realtime->destination_name);
	free((void*)realtime);
}