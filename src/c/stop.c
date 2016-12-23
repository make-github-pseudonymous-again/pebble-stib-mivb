#include <pebble.h>
#include "ds/dynamicarray.h"
#include "stop.h"
#include "realtime.h"
#include "std/strdup.h"

Stop* Stop_create(
	const uint32_t stop_id,
	const char *stop_name
) {

	Stop *stop = malloc(sizeof(Stop));

	if (stop == NULL) return NULL;

	const char *stop_name_copy = strdup(stop_name);

	*stop = (Stop) {
		.stop_id = stop_id,
		.stop_name = stop_name_copy,
		.realtimes = realtimes
	};

	return stop;
}

void Stop_free(const Stop *stop) {

	size_t n = stop->realtimes.length;
	for (size_t i = 0; i < n; ++i) Realtime_free(stop->realtimes.data[i]);
	ds_DynamicArray_free(&stop->realtimes);

	free((void*)stop->stop_name);
	free((void*)stop);

}
