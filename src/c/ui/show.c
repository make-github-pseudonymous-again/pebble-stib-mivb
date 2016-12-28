#include <pebble.h>
#include "show.h"
#include "draw.h"
#include "main_window.h"
#include "../data/stops.h"

void show_next ( ) {

  ++ui_displayed_stop_index;

  if ( ui_displayed_stop_index >= data_stops_curr.length ) {
    // cycle
    ui_displayed_stop_index = 0 ;
  }

  clear();
  draw();

}

void show_closest ( ) {

  ui_displayed_stop_index = 0;
  clear();
  draw();

}
