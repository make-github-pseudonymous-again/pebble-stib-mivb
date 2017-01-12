#pragma once
#include <pebble.h>

#define INBOX_SIZE 256

//extern time_t inbox_last_loaded_event_ts;

void inbox_dropped_callback(AppMessageResult reason, void *context);
void inbox_received_callback(DictionaryIterator *iterator, void *context);