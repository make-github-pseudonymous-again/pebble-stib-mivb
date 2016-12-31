#include <pebble.h>
#include "stops.h"
#include "../ds/dynamicarray.h"
#include "stop.h"

#define STOPS_PERSIST_KEY_BEGIN 1

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

status_t persist_write_string_trunc(const uint32_t key, const char *cstring){
  const size_t n = strlen(cstring);
  const size_t m = n < PERSIST_DATA_MAX_LENGTH ? n : PERSIST_DATA_MAX_LENGTH;
  return persist_write_data(key, cstring, m);
}

status_t persist_read_string_trunc(const uint32_t key, char *pt){
  int size = persist_get_size(key);
  if (size < 0) return size;
  pt = malloc(size+1);
  status_t status = persist_read_data(key, pt, size);
  pt[size]='\0';
  return status;
}
  

void Stops_persist_write(const ds_DynamicArray *stops){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] write(%p)", stops);
  status_t total = 0;
  status_t status;
  uint32_t key = STOPS_PERSIST_KEY_BEGIN;
  
  const size_t n = stops->length;
  
  status = persist_write_int(key++, n);
  if (status<0) return status;
  total += status;
  
  for (size_t i = 0; i < n; ++i){
    
    Stop *stop = stops->data[i];
    
    status = Stop_write(&key, stop);
    if (status<0) return status;
    total += status;
    
  }
}

uint32_t Stops_persist_read(const ds_DynamicArray *stops){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] read(%p)", stops);
  status_t status;
  uint32_t key = STOPS_PERSIST_KEY_BEGIN;
  const size_t n = persist_read_int(key++); // defaults to zero
  for (size_t i = 0; i < n; ++i){
    Stop *stop = Stop_read(&key);
    ds_DynamicArray_push(stops, stop);
  }
  return key;
}

void Stops_persist_clear(const uint32_t hi){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[stops] clear(%lu)", hi);
  for (size_t key = STOPS_PERSIST_KEY_BEGIN; key < hi; ++key){
    status_t status = persist_delete(key);
    if (status<0) APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to delete key %lu", key);
  } 
}