#include <pebble.h>
#include "ui/main_window.h"
#include "ui/tick.h"
#include "msg/inbox.h"
#include "msg/outbox.h"

static void init() {
  // Create main window element and assign to pointer
  ui_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(ui_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the window on the watch with animated=true
  window_stack_push(ui_main_window, true);

  // Register with TickTimerService
  // (using minute instead of second uses less power)
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(INBOX_SIZE, OUTBOX_SIZE);
}

static void deinit() {
  // Destroy Window
  window_destroy(ui_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}