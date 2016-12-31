#include <pebble.h>
#include "show.h"
#include "draw.h"
#include "main_window.h"
#include "scroll.h"
#include "../data/stops.h"

void show_next ( ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[show] next");

  scroll_reset();

  ++ui_displayed_stop_index;

  if ( ui_displayed_stop_index >= data_stops_curr.length ) {
    // cycle
    ui_displayed_stop_index = 0 ;
  }
  
  if (data_stops_curr.length > 0) {
    Stop *stop = data_stops_curr.data[ui_displayed_stop_index];
    ui_displayed_stop_id = stop->id;
  }

  clear();
  draw();

}

void show_closest ( ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[show] closest");

  scroll_reset();
  
  ui_displayed_stop_index = 0;
  
  if (data_stops_curr.length > 0) {
    Stop *stop = data_stops_curr.data[ui_displayed_stop_index];
    ui_displayed_stop_id = stop->id;
  }
  
  clear();
  draw();

}
