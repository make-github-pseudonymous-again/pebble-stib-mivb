#pragma once
#include <pebble.h>
#include "stop.h"
#include "../ds/dynamicarray.h"

// list with displayed data
ds_DynamicArray data_stops_curr;
// list with data being received
ds_DynamicArray data_stops_recv;

Stop* get_stop(const ds_DynamicArray *stops, const uint32_t stop_id);
void Stops_clear(ds_DynamicArray *stops);