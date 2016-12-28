#include <pebble.h>
#include "scroll.h"

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

  else {

    const size_t n = s_stops.data[s_displayed_stop_index]->realtime->results->length;
    const size_t r = n % DISPLAYED_ITEMS ;
    const size_t c = n / DISPLAYED_ITEMS ;
    const size_t b = c + ((r>0)?1:0);

    if (s_scroll >= b) s_scroll = b-1;

  }

}
