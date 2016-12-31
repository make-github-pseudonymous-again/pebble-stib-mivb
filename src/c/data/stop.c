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

status_t Stop_write(const uint32_t *key, const Stop *stop) {
  status_t total = 0;
  status_t status;
  status = persist_write_int((*key)++, stop->id);
  if (status<0) return status;
  total += status;
  status = persist_write_string_trunc((*key)++, stop->name);
  if (status<0) return status;
  total += status;
  status = persist_write_int((*key)++, stop->error);
  if (status<0) return status;
  total += status;
  if(stop->message!=NULL) {
    status = persist_write_string_trunc((*key)++, stop->message);
    if (status<0) return status;
    total += status;
  }
  else {
    (*key)++;
  }

  const size_t m = stop->realtime.length;
  
  status = persist_write_int((*key)++, m);
  if (status<0) return status;
  total += status;
  
  for (size_t j = 0; j < m; ++j) {
    
    Realtime *realtime = stop->realtime.data[j];
    
    status = Realtime_write(&key, realtime);
    if (status<0) return status;
    total += status;
    
  }
  
  return total;
}

Stop* Stop_read(const uint32_t *key) {

	Stop *stop = malloc(sizeof(Stop));

	if (stop == NULL) return NULL;
  
  status_t status;
  
  stop->id = persist_read_int((*key)++);
  
  status = persist_read_string_trunc((*key)++, stop->name);
  
  stop->error = persist_read_int((*key)++);
  
  stop->message = NULL; // default message
  status = persist_read_string_trunc((*key)++, stop->message);

  ds_DynamicArray_init(&stop->realtime, 1);
  
  const size_t m = persist_read_int((*key)++);
  
  for (size_t j = 0; j < m; ++j) {
    Realtime *realtime = Realtime_read(&key, stop->id);
    ds_DynamicArray_push(&stop->realtime, realtime);
  }

	return stop;
}