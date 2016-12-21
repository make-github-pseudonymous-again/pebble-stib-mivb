#include <pebble.h>
#include "realtime.h"
#include "std/strdup.h"

(Realtime) {
	.stop_id = stop_id,
	.stop_name = stop_name,
	.line_name = line_name,
	.destination_name = destination_name,
	.foreground_color = foreground_color,
	.background_color = background_color,
	.utc = utc
};

Realtime* Realtime_create(
	const uint32_t stop_id,
	const char *stop_name,
	const char *line_name,
	const char *destination_name,
	const uint32_t foreground_color,
	const uint32_t background_color,
	const uint32_t utc	
) {
	Realtime* s = malloc(sizeof(Realtime));
	
	char *stop_name_copy = strdup(stop_name);
	char *line_name_copy = strdup(line_name);
	char *destination_name_copy = strdup(destination_name);

  *s = (Station) {
    .free = Station_free,
    .copy = Station_copy,
    .name = malloc(sizeof(char) * (name_length + 1)),
    .empty_slots = empty_slots,
    .free_bike = free_bike,
    .distance = distance,
    .angle = DEG_TO_TRIGANGLE(angle),
  };
  memcpy(s->name, name, name_length);
  s->name[name_length] = '\0';

return s;
}