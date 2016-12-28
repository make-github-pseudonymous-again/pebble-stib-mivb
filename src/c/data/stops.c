#include <pebble.h>
#include "stops.h"
#include "../ds/dynamicarray.h"
#include "stop.h"

ds_DynamicArray data_stops_curr;
ds_DynamicArray data_stops_recv;

Stop* get_stop(const ds_DynamicArray *stops, const uint32_t stop_id) {

  const size_t n = stops->length;

  for (size_t i = 0 ; i < n ; ++i){
  	Stop *stop = stops->data[i];
  	if (stop->id == stop_id) {
  	  return stop;
  	}
  }

  return NULL;

}

void Stops_clear(ds_DynamicArray *stops) {

  const size_t n = stops->length;

  for (size_t i = 0 ; i < n ; ++i){
  	Stop *stop = stops->data[i];
  	Stop_destroy(stop);
  }

  ds_DynamicArray_clear(stops);

}
