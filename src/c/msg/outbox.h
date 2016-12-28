#pragma once
#include <pebble.h>

const int OUTBOX_SIZE = 256;

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

void outbox_sent_callback(DictionaryIterator *iterator, void *context);
