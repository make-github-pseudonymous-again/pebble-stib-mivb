#include <pebble.h>
#include "stops.h"
#include "../ds/dynamicarray.h"
#include "stop.h"

#define STOPS_PERSIST_KEY_BEGIN 1

ds_DynamicArray data_stops_curr;
ds_DynamicArray data_stops_recv;

Stop* get_stop(const ds_DynamicArray *stops, const uint32_t stop_id) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] get_stop(%p,%u)", stops, stop_id);
  
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
  
  status = persist_write_int((*key)++, n);
  if (status<0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to write stop count %lu, error %d", n, status);
    return status;
  }
  total += status;
  
  for (size_t i = 0; i < n; ++i){
    
    Stop *stop = stops->data[i];
    const uint32_t begin = *key;
    status = Stop_persist_write(key, stop);
    if (status<0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to write stop %lu, error %d", i, status);
      Stops_persist_clear(begin, *key);
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
      APP_LOG(APP_LOG_LEVEL_ERROR, "[stop] persist_read > failed to read stop %lu at key %u", i, *key);
      Stop_persist_skip(key);
    }
    else {
      ds_DynamicArray_push(stops, stop);
    }
  }
  return *key;
}

void Stops_persist_clear(const uint32_t lo, const uint32_t hi){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] clear(%u, %u)", lo, hi);
  for (uint32_t key = lo; key < hi; ++key){
    status_t status = persist_delete(key);
    if (status != S_SUCCESS) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to delete key %u, error %d", key, status);
    }
  } 
}


uint32_t freeze(){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] freeze");
  uint32_t key = STOPS_PERSIST_KEY_BEGIN;
  return Stops_persist_write(&key, &data_stops_curr);
}

uint32_t thaw(){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] thaw");
  uint32_t key = STOPS_PERSIST_KEY_BEGIN;
  return Stops_persist_read(&key, &data_stops_curr);
}