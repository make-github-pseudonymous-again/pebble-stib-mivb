#include <pebble.h>
#include "tick.h"
#include "draw.h"

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[tick] handler");
  clear();
  draw_from_tm(false, tick_time);
}