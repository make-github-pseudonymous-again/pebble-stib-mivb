#include <pebble.h>
#include "scroll.h"
#include "draw.h"
#include "main_window.h"
#include "../data/stops.h"

static int16_t s_scroll = 0;

int16_t get_scroll(){
  return s_scroll;
}

void scroll_up(){
  --s_scroll;
  clear();
  draw();
}

void scroll_down(){
  ++s_scroll;
  clear();
  draw();
}

void scroll_fix(){

  if (s_scroll < 0) s_scroll = 0;
  
  else if (data_stops_curr.length == 0) s_scroll = 0;

  else {

    const Stop *stop = data_stops_curr.data[ui_displayed_stop_index];
    const size_t n = stop->realtime.length;
    // TODO avoid overflow
    const int16_t r = n % SCROLL_STEP ;
    const int16_t c = n / SCROLL_STEP ;
    const int16_t b = c + ((r>0)?1:0);

    if (s_scroll >= b) s_scroll = b-1;

  }

}

void scroll_reset() {
  s_scroll = 0;
}