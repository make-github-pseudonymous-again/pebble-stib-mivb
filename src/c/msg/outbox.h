#pragma once
#include <pebble.h>

#define OUTBOX_SIZE 128

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
void outbox_sent_callback(DictionaryIterator *iterator, void *context);