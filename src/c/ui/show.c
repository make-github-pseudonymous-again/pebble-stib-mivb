#include <pebble.h>
#include "show.h"
#include "draw.h"
#include "main_window.h"
#include "scroll.h"
#include "../data/stops.h"

void show_next ( ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "show_next");

  scroll_reset();

  ++ui_displayed_stop_index;

  if ( ui_displayed_stop_index >= data_stops_curr.length ) {
    // cycle
    ui_displayed_stop_index = 0 ;
  }
  
  update_displayed_stop_id();

  clear();
  draw();

}

void show_closest ( ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "show_closest");

  scroll_reset();
  
  ui_displayed_stop_index = 0;
  
  update_displayed_stop_id();
  
  clear();
  draw();

}
