#include <pebble.h>
#include "outbox.h"
#include "../pebble/translate_error.h"

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  int size = (int)iterator->end - (int)iterator->dictionary;
  APP_LOG(APP_LOG_LEVEL_ERROR, "[outbox] Failed to send %d bytes: %i - %s", size, reason, pebble_translate_app_message_error(reason));
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  int size = (int)iterator->end - (int)iterator->dictionary;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[outbox] Sent %d bytes", size);
}
