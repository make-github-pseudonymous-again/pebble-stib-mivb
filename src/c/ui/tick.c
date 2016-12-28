#include <pebble.h>
#include "ui/tick.h"
#include "ui/draw.h"

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  draw_from_tm(tick_time);
}
