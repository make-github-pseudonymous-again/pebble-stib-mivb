#pragma once
#include <pebble.h>
#include "ds/dynamicarray.h"

Stop* get_stop(const ds_DynamicArray *stops, const uint32_t stop_id);
void Stops_clear(const ds_DynamicArray *stops);
