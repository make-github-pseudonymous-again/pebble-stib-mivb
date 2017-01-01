#include <pebble.h>
#include "draw.h"
#include "when.h"
#include "scroll.h"
#include "main_window.h"
#include "color.h"
#include "../data/realtime.h"
#include "../data/stop.h"
#include "../data/stops.h"
#include "../msg/inbox.h"

void draw_from_time(bool quiet, const time_t now) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "draw_from_time(%s, %ld)", quiet ? "true" : "false", now);

  scroll_fix();

  Layer *window_layer = window_get_root_layer(ui_main_window);
  
  // add status bar
  layer_add_child(window_layer, status_bar_layer_get_layer(ui_status_bar));

  if (data_stops_curr.length == 0){
    handle_error("NO DATA", "nothing to display");
    return;
  }

  Stop *displayed_stop = data_stops_curr.data[ui_displayed_stop_index];

  ui_displayed_stop_id = displayed_stop->id ;

  text_layer_set_text(ui_stop_name_layer, displayed_stop->name);
  layer_add_child(window_layer, text_layer_get_layer(ui_stop_name_layer));

  if ( displayed_stop->error != 0 ) {
    text_layer_set_text(ui_message_layer, displayed_stop->message);
    layer_add_child(window_layer, text_layer_get_layer(ui_message_layer));
    return;
  }

  if ( displayed_stop->realtime.length == 0 ) {
    text_layer_set_text(ui_message_layer, MESSAGE_NOTHING);
    layer_add_child(window_layer, text_layer_get_layer(ui_message_layer));
    return;
  }

  else {

    for ( size_t i = 0; i < DISPLAYED_ITEMS ; ++i ) {

      const size_t j = i + get_scroll() * SCROLL_STEP;

      if ( j == displayed_stop->realtime.length ) break ;

      Realtime* realtime = displayed_stop->realtime.data[j] ;
      draw_realtime_item(quiet, window_layer, now, i, realtime);

    }

  }

}

void draw_from_tm(bool quiet, struct tm *tick_time) {
  const time_t now = mktime(tick_time);
  draw_from_time(quiet, now);
}

void draw(bool quiet) {
  const time_t now = time(NULL);
  draw_from_time(quiet, now);
}

void draw_realtime_item(bool quiet, Layer* root, const time_t now, const size_t i, const Realtime* realtime){

  char *minutes_buffer = ui_minutes_buffer[i];
  
  quiet = quiet && (i != 0); // if first item on the list, vibrate if close or gone
  GColor when_color = when(minutes_buffer, now, realtime->utc, quiet) ;

  const int16_t offset = i*LINEHEIGHT;
  const int16_t l = get_main_window_left();
  const int16_t t = get_main_window_top() + get_main_window_title() + 3;
  const int16_t w = get_main_window_width();

  TextLayer* line_number_layer = text_layer_create(GRect(l, t+offset, BOXWIDTH, BOXHEIGHT));
  text_layer_set_font(line_number_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(line_number_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(line_number_layer, GTextOverflowModeFill);
  text_layer_set_text(line_number_layer, realtime->line_number);
  text_layer_set_text_color(line_number_layer, GColorFromHEX(realtime->foreground_color));
  text_layer_set_background_color(line_number_layer, GColorFromHEX(realtime->background_color));
  ui_line_number_layer[i] = line_number_layer;

  TextLayer* destination_name_layer = text_layer_create(GRect(l+BOXWIDTH+5, t+offset, w-2*l-MINUTES_WIDTH-BOXWIDTH, BOXHEIGHT));
  text_layer_set_font(destination_name_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(destination_name_layer, GTextAlignmentLeft);
  text_layer_set_overflow_mode(destination_name_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text(destination_name_layer, realtime->destination_name);
  text_layer_set_text_color(destination_name_layer, GColorBlack);
  ui_destination_name_layer[i] = destination_name_layer;

  TextLayer* minutes_layer = text_layer_create(GRect(w-MINUTES_WIDTH-l, t+offset, MINUTES_WIDTH, BOXHEIGHT));
  text_layer_set_font(minutes_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(minutes_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(minutes_layer, GTextOverflowModeFill);
  text_layer_set_text(minutes_layer, minutes_buffer);
  text_layer_set_text_color(minutes_layer, when_color);
  ui_minutes_layer[i] = minutes_layer;

  layer_add_child(root, text_layer_get_layer(line_number_layer));
  layer_add_child(root, text_layer_get_layer(destination_name_layer));
  layer_add_child(root, text_layer_get_layer(minutes_layer));

}

void clear() {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "clear");
  
  Layer *window_layer = window_get_root_layer(ui_main_window);
  layer_remove_child_layers(window_layer);

  for (size_t i = 0; i < DISPLAYED_ITEMS; ++i) {
    text_layer_destroy(ui_line_number_layer[i]);
    text_layer_destroy(ui_destination_name_layer[i]);
    text_layer_destroy(ui_minutes_layer[i]);
    ui_line_number_layer[i] = NULL;
    ui_destination_name_layer[i] = NULL;
    ui_minutes_layer[i] = NULL;
  }

}

void handle_error ( const char* title , const char* message ) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_error: %s %s", title, message);

  const time_t now = time(NULL);
  const time_t time_since_last_loaded_event = now - inbox_last_loaded_event_ts;
  if ( time_since_last_loaded_event < TKO ) {
    status_bar_layer_set_colors(ui_status_bar, BOK, FOK);
    return ;
  }

  status_bar_layer_set_colors(ui_status_bar, BKO, FKO);

  Layer *window_layer = window_get_root_layer(ui_main_window);

  text_layer_set_text(ui_stop_name_layer, title);
  text_layer_set_text(ui_message_layer, message);
  layer_add_child(window_layer, text_layer_get_layer(ui_stop_name_layer));
  layer_add_child(window_layer, text_layer_get_layer(ui_message_layer));

}
