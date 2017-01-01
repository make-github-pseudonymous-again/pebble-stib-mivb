#pragma once
#include <pebble.h>
#include "stop.h"
#include "../ds/dynamicarray.h"

#define STOPS_PERSIST_KEY_BEGIN 1

// list with displayed data
extern ds_DynamicArray data_stops_curr;
// list with data being received
extern ds_DynamicArray data_stops_recv;

Stop* get_stop(const ds_DynamicArray *stops, const uint32_t stop_id);
void Stops_clear(ds_DynamicArray *stops);

// persist
status_t Stops_persist_write(uint32_t *key, const ds_DynamicArray *stops);
uint32_t Stops_persist_read(uint32_t *key, ds_DynamicArray *stops);
void Stops_persist_clear(const uint32_t lo, const uint32_t hi);