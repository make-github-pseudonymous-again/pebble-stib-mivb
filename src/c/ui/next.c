#include <pebble.h>
#include "next.h"

void next ( ) {

  ++s_displayed_stop_index;

  if ( s_displayed_stop_index >= s_stops.length ) {
    // cycle
    s_displayed_stop_index = 0 ;
  }

  clear();
  draw();

}
