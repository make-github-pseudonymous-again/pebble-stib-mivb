#include <pebble.h>
#include "stop.h"
#include "realtime.h"
#include "../ds/dynamicarray.h"
#include "../std/strdup.h"

Stop* Stop_create(
	const uint32_t id,
	const char *name,
	const uint32_t error,
	const char *message
) {

	Stop *stop = malloc(sizeof(Stop));

	if (stop == NULL) return NULL;

	const char *name_copy = strdup(name);
  const char *message_copy = NULL;
  if (message != NULL) message_copy = strdup(message);

	*stop = (Stop) {
		.id = id,
		.name = name_copy,
		.error = error,
		.message = message_copy
	};
  
  ds_DynamicArray_init(&stop->realtime, 1);

	return stop;
}

void Stop_destroy(Stop *stop) {

	size_t n = stop->realtime.length;
	for (size_t i = 0; i < n; ++i) {
		Realtime *realtime = stop->realtime.data[i];
		Realtime_destroy(realtime);
	}
	ds_DynamicArray_clear(&stop->realtime);

	free((void*)stop->name);
	free((void*)stop->message);
	free((void*)stop);

}
