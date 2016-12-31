#include <pebble.h>
#include "realtime.h"
#include "../std/strdup.h"

Realtime* Realtime_create(
	const uint32_t stop_id,
	const char *line_number,
	const char *destination_name,
	const uint32_t foreground_color,
	const uint32_t background_color,
	const uint32_t utc
) {
	Realtime *realtime = malloc(sizeof(Realtime));

	if (realtime == NULL) return NULL;

	const char *line_number_copy = strdup(line_number);
	const char *destination_name_copy = strdup(destination_name);

	*realtime = (Realtime) {
		.stop_id = stop_id,
		.line_number = line_number_copy,
		.destination_name = destination_name_copy,
		.foreground_color = foreground_color,
		.background_color = background_color,
		.utc = utc
	};

	return realtime;
}

void Realtime_destroy(Realtime *realtime) {
	free((void*)realtime->line_number);
	free((void*)realtime->destination_name);
	free((void*)realtime);
}


status_t Realtime_write(const uint32_t *key, const Realtime *realtime){
  status_t total = 0;
  status_t status;
  // writing stop_id is silly
  // stop_id will be reconstructed from parent stop data
  // status = persist_write_int((*key)++, realtime->stop_id);
  // if (status<0) return status;
  // total += status;
  status = persist_write_string_trunc((*key)++, realtime->line_number);
  if (status<0) return status;
  total += status;
  status = persist_write_string_trunc((*key)++, realtime->destination_name);
  if (status<0) return status;
  total += status;
  status = persist_write_int((*key)++, realtime->foreground_color);
  if (status<0) return status;
  total += status;
  status = persist_write_int((*key)++, realtime->background_color);
  if (status<0) return status;
  total += status;
  status = persist_write_int((*key)++, realtime->utc);
  if (status<0) return status;
  total += status;
  return total;
}

Realtime* Realtime_read(const uint32_t *key, const uint32_t stop_id) {

	Realtime *realtime = malloc(sizeof(Realtime));

	if (realtime == NULL) return NULL;
  
  status_t status;
  
  realtime->stop_id = stop_id; // instead of reading it
  
  status = persist_read_string_trunc((*key)++, realtime->line_number);
  status = persist_read_string_trunc((*key)++, realtime->destination_name);
  stop->foreground_color = persist_read_int((*key)++);
  stop->background_color = persist_read_int((*key)++);
  stop->utc = persist_read_int((*key)++);

	return realtime;
}