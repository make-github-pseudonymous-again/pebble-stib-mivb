#include <pebble.h>
#include "draw.h"
#include "when.h"
#include "scroll.h"
#include "main_window.h"
#include "../data/realtime.h"
#include "../data/stop.h"
#include "../data/stops.h"

void draw_from_time(const time_t now) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "[draw] draw_from_time(%d)", now);

  scroll_fix();

  Layer *window_layer = window_get_root_layer(ui_main_window);

  if (data_stops_curr.length == 0){
    // TODO handle no data drawing
  }

  else{

    Stop *displayed_stop = data_stops_curr.data[s_displayed_stop_index];

    s_displayed_stop_id = displayed_stop->id ;

    text_layer_set_text(s_stop_name_layer, displayed_stop->name);
    layer_add_child(window_layer, text_layer_get_layer(s_stop_name_layer));

    if ( displayed_stop->error ) {
      text_layer_set_text(s_message_layer, displayed_stop->message);
      layer_add_child(window_layer, text_layer_get_layer(s_message_layer));
      return;
    }

    if ( displayed_stop->realtimes->length == 0 ) {
      text_layer_set_text(s_message_layer, MESSAGE_NOTHING);
      layer_add_child(window_layer, text_layer_get_layer(s_message_layer));
      return;
    }

    else {

      for ( size_t i = 0; i < DISPLAYED_ITEMS ; ++i ) {

	const size_t j = i + s_scroll * DISPLAYED_ITEMS;

	if ( j == displayed_stop->realtimes.length ) break ;

	Realtime* realtime = displayed_stop->realtimes.data[j] ;
	draw_realtime_item(now, i, realtime);

      }

    }

  }

}

void draw_from_tm(struct tm *tick_time) {
  const time_t now = mktime(tick_time);
  draw_from_time(now);
}

void draw() {
  const time_t now = time(NULL);
  draw_from_time(now);
}

void draw_realtime_item(const time_t now, const size_t i, const Realtime* realtime){

  char *minutes_buffer = s_minutes_buffer[i];
  GColor when_color = when(minutes_buffer, now, realtime->utc) ;

  const int16_t offset = i*LINEHEIGHT ;

  TextLayer* number_layer = text_layer_create(GRect(s_left, s_top+offset, BOXHEIGHT, BOXHEIGHT));
  text_layer_set_font(number_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(number_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(number_layer, GTextOverflowModeFill);
  text_layer_set_text(number_layer, realtime->line_number);
  text_layer_set_text_color(number_layer, GColorFromHex(realtime->foreground_color));
  text_layer_set_background_color(number_layer, GColorFromHex(realtime->background_color));
  s_number_layer[i] = number_layer;

  TextLayer* destination_layer = text_layer_create(GRect(s_left+37, s_top+offset, s_w-91, 20));
  text_layer_set_font(destination_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(destination_layer, GTextAlignmentLeft);
  text_layer_set_overflow_mode(destination_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text(destination_layer, realtime->destination_name);
  text_layer_set_text_color(destination_layer, GColorBlack);
  s_destination_layer[i] = destination_layer;

  TextLayer* minutes_layer = text_layer_create(GRect(s_left+s_w-54, s_top+offset, 22, 20));
  text_layer_set_font(minutes_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(minutes_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(minutes_layer, GTextOverflowModeFill);
  text_layer_set_text(minutes_layer, minutes_buffer);
  text_layer_set_text_color(minutes_layer, when_color);
  s_minutes_layer[i] = minutes_layer;

  layer_add_child(window_layer, number_layer);
  layer_add_child(window_layer, destination_layer);
  layer_add_child(window_layer, minutes_layer);

}

void clear ( ) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  layer_remove_child_layers(window_layer);

  for (size_t i = 0; i < DISPLAYED_ITEMS; ++i) {
    text_layer_destroy(s_number_layer[i]);
    text_layer_destroy(s_destination_layer[i]);
    text_layer_destroy(s_minutes_layer[i]);
    s_number_layer[i] = NULL;
    s_destination_layer[i] = NULL;
    s_minutes_layer[i] = NULL;
  }

  // TODO delete everything!
}

void handle_error ( const char* title , const char* message ) {

  APP_LOG(APP_LOG_LEVEL_ERROR, "handle_error:", title, message);

  const time_t now = time(NULL);

  if ( now - (time_t)s_uuid_send_realtime < TKO ) {
    status_bar_layer_set_colors(s_status_bar, BOK, FOK);
    return ;
  }

  clear();
  status_bar_layer_set_colors(s_status_bar, BKO, FKO);

  Layer *window_layer = window_get_root_layer(s_main_window);

  text_layer_set_text(s_stop_name_layer, title);
  text_layer_set_text(s_message_layer, message);
  layer_add_child(window_layer, text_layer_get_layer(s_stop_name_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_message_layer));

}
