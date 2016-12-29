#include <pebble.h>
#include "tick.h"
#include "draw.h"

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  clear();
  draw_from_tm(tick_time);
}