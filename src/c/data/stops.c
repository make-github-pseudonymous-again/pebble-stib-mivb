#include <pebble.h>
#include "stops.h"
#include "stop.h"
#include "../ds/dynamicarray.h"
#include "../pebble/translate_error.h"

ds_DynamicArray data_stops_curr;
ds_DynamicArray data_stops_recv;

Stop* get_stop(const ds_DynamicArray *stops, const uint32_t stop_id) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] get_stop(%p,%lu)", stops, stop_id);
  
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] Stops_clear(%p)", stops);

  const size_t n = stops->length;

  for (size_t i = 0 ; i < n ; ++i){
  	Stop *stop = stops->data[i];
  	Stop_destroy(stop);
  }

  ds_DynamicArray_clear(stops);

}
  
status_t Stops_persist_write(uint32_t *key, const ds_DynamicArray *stops){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] write(%p)", stops);
  status_t total = 0;
  status_t status;
  
  const size_t n = stops->length;
  const uint32_t n_pos = *key;
  
  status = persist_write_int((*key)++, n);
  if (status<0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to write stop count %u, error %s", n, pebble_translate_status_error(status));
    return status;
  }
  total += status;
  
  for (size_t i = 0; i < n; ++i){
    
    Stop *stop = stops->data[i];
    const uint32_t begin = *key;
    status = Stop_persist_write(key, stop);
    if (status<0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to write stop %u, error %s", i, pebble_translate_status_error(status));
      Stops_persist_clear(begin, *key);
      persist_write_int(n_pos, i); // overwrite number of items
      return status;
    }
    total += status;
    
  }
  
  return total;
}

uint32_t Stops_persist_read(uint32_t *key, ds_DynamicArray *stops){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] read(%p)", stops);
  const size_t n = persist_read_int((*key)++); // defaults to zero
  for (size_t i = 0; i < n; ++i){
    Stop *stop = Stop_persist_read(key);
    if (stop == NULL) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "[stop] persist_read > failed to read stop %u at key %lu", i, *key);
      Stop_persist_skip(key);
    }
    else {
      ds_DynamicArray_push(stops, stop);
    }
  }
  return *key;
}

void Stops_persist_clear(const uint32_t lo, const uint32_t hi){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] clear(%lu, %lu)", lo, hi);
  for (uint32_t key = lo; key < hi; ++key){
    status_t status = persist_delete(key);
    if (status<0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to delete key %lu, error %s", key, pebble_translate_status_error(status));
    }
  } 
}