include <pebble.h>
#include "when.h"
#include "stops.h"
#include "stop.h"
#include "realtime.h"
#include "ds/bisect.h"
#include "ds/dynamicarray.h"
#include "pebble/translate_error.h"

// CONSTANTS

static const uint32_t VAL_TYPE_STATE = 0;
static const uint32_t VAL_TYPE_REALTIME_TIME = 1;
static const uint32_t VAL_TYPE_REALTIME_STOP = 2;
static const uint32_t VAL_TYPE_REALTIME_END = 3;

// KO timeout
static const time_t TKO = 60000;

// colors
static const GColor BOK = GColorMayGreen;
static const GColor FOK = GColorWhite;
static const GColor BLO = GColorIcterine;
static const GColor FLO = GColorBlack;
static const GColor BKO = GColorFolly;
static const GColor FKO = GColorWhite;
static const GColor BNG = GColorChromYellow;
static const GColor FNG = GColorWhite;
static const GColor  BG = GColorWhite;


static const int16_t BOXHEIGHT = 32;
static const int16_t MARGIN = 3;
static const int16_t LINEHEIGHT = BOXHEIGHT + MARGIN;

static const int16_t MARING_LEFT = 15;
static const int16_t MARING_TOP = 30;

// font
static const char *FONT = FONT_KEY_GOTHIC_24_BOLD;

static const size_t DISPLAYED_ITEMS = 2;
static const char *MESSAGE_NOTHING = "nothing right now";

// GLOBALS
static int16_t s_scroll = 0;
static uint32_t s_last_seen_stop_id;
// list with displayed data
static ds_DynamicArray s_stops;
// list with data being received
static ds_DynamicArray s_stops_recv;

// memory necessary to display realtime
static char s_minutes_buffer[DISPLAYED_ITEMS][WHEN_BUFFER_SIZE];

// dynamically allocated and freed memory for layers displaying realtime
static TextLayer* s_line_number_layer[DISPLAYED_ITEMS] = { NULL };
static TextLayer* s_destination_name_layer[DISPLAYED_ITEMS] = { NULL };
static TextLayer* s_minutes_layer[DISPLAYED_ITEMS] = { NULL };

// used to remember which stop we are displaying
static uint32_t s_displayed_stop_id = 0;
static size_t s_displayed_stop_index = 0;

// initialized once, deleted on app kill
static Window *s_main_window = NULL;
// initialized once, deleted on app kill
static StatusBarLayer *s_status_bar = NULL;
// initialized once, deleted on app kill
static TextLayer *s_info_layer = NULL;
// initialized once, deleted on app kill
static TextLayer *s_stop_name_layer = NULL;
// initialized once, deleted on app kill
static TextLayer *s_message_layer = NULL;

// used to keep track of last received message
static uint32_t s_uuid_run;
static uint32_t s_uuid_send_realtime;

// to store the dimensions of the main window
static GRect s_bounds;
static GSize s_size;
static int16_t s_w;
static int16_t s_h;


static void ad ( Layer* layer ) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  layer_add_child(window_layer, layer);
}

static void clear ( ) {
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

static void display_realtime_item(const time_t now, const size_t i, const Realtime* realtime){

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

static void draw_from_time(const time_t now) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "draw_from_time");

  scroll_fix();

  Layer *window_layer = window_get_root_layer(s_main_window);

  Stop *displayed_stop = s_stops.data[s_displayed_stop_index];

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
      display_realtime_item(now, i, realtime);

    }

  }

}

static void draw_from_tm(struct tm *tick_time) {
  const time_t now = mktime(tick_time);
  draw_from_time(now);
}

static void draw() {
  const time_t now = time(NULL);
  draw_from_time(now);
}

static void scroll_up(){
  ++s_scroll;
  clear();
  draw();
}

static void scroll_down(){
  --s_scroll;
  clear();
  draw();
}

static void scroll_fix(){

  if (s_scroll < 0) s_scroll = 0;

  else {

    const size_t n = s_stops.data[s_displayed_stop_index]->realtime->results->length;
    const size_t r = n % DISPLAYED_ITEMS ;
    const size_t c = n / DISPLAYED_ITEMS ;
    const size_t b = c + ((r>0)?1:0);

    if (s_scroll >= b) s_scroll = b-1;

  }

}



static void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "single click select");
  other();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "long click select");
  load( null , true );
}

static void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "single click down");
  scroll_down();
}

static void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "single click up");
  scroll_up();
}

static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_single_click_handler);
}

static void main_window_load(Window *window) {

  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(window_layer);

  s_size = s_bounds.size ;
  s_w = s_size.w ;
  s_h = s_size.h ;
  s_left = MARGIN_LEFT;
  s_top = MARGIN_TOP;

  // Set background color
  window_set_background_color(window, BG);

  // Create the StatusBarLayer
  s_status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(s_status_bar, BKO, FKO);
  status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

  // Stop name text layer
  s_stop_name_layer = text_layer_create(GRect(25, 0, s_w-50, 20));
  text_layer_set_font(s_stop_name_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(s_stop_name_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_stop_name_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text_color(s_stop_name_layer, GColorBlack);

  // Message text layer
  s_message_layer = text_layer_create(GRect(s_left, s_top, s_w - 34, s_h - s_top));
  text_layer_set_font(s_stop_name_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(s_stop_name_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_stop_name_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_color(s_stop_name_layer, BKO);

  // Update display with cached information
  draw();

  // Register click events
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

}

static void main_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload");
  status_bar_layer_destroy(s_status_bar);
  text_layer_destroy(s_stop_name_layer);
  text_layer_destroy(s_message_layer);
  // TODO cache data on the watch
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  draw_from_tm(tick_time);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  switch (dict_find(iterator, MESSAGE_KEY_TYPE)->value->uint32) {
    case VAL_TYPE_REALTIME_STOP: {
      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
      //const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;

      if ( uuid_run < s_uuid_run || ( uuid_run == s_uuid_run && uuid_send_realtime < s_uuid_send_realtime ) ) {
	      // this should  never happen
	      APP_LOG(APP_LOG_LEVEL_ERROR, "received old realtime message");
	      break;
      }

      if ( uuid_run > s_uuid_run || uuid_send_realtime > s_uuid_send_realtime ) {
	      // we are receiving new realtime data, drop old
	      APP_LOG(APP_LOG_LEVEL_DEBUG, "receiving new realtime data");
	      s_uuid_run = uuid_run;
	      s_uuid_send_realtime = uuid_send_realtime;
	      Stops_clear(&s_stops_recv);
      }

      const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
      const char *stop_name = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_NAME)->value->cstring;
      const uint32_t stop_error = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ERROR)->value->uint32;
      const char *stop_message = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_MESSAGE)->value->cstring;

      Stop* stop = Stop_create(stop_id, stop_name, stop_error, stop_message);
      ds_DynamicArray_push(&s_stops_recv, stop);

      break;

    }
    case VAL_TYPE_REALTIME_REALTIME: {

      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
      //const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;

      if ( uuid_run != UUID_RUN || uuid_send_realtime != UUID_SEND_REALTIME ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "received wrong realtime uuid");
	break;
      }

      const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
      const char *line_name = dict_find(iterator, MESSAGE_KEY_REALTIME_LINE_NAME)->value->cstring;
      const char *destination_name = dict_find(iterator, MESSAGE_KEY_REALTIME_DESTINATION_NAME)->value->cstring;
      const uint32_t foreground_color = dict_find(iterator, MESSAGE_KEY_REALTIME_FOREGROUND_COLOR)->value->uint32;
      const uint32_t background_color = dict_find(iterator, MESSAGE_KEY_REALTIME_BACKGROUND_COLOR)->value->uint32;
      const uint32_t utc = dict_find(iterator, MESSAGE_KEY_REALTIME_UTC)->value->uint32;

      Realtime *realtime = Realtime_create(
	stop_id,
	line_name,
	destination_name,
	foreground_color,
	background_color,
	utc
      );

      Stop *stop = get_stop(&s_stops_recv, stop_id);

      if ( stop == NULL ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "stop object missing");
	break;
      }

      ds_DynamicArray_push(&stop->realtime, realtime);
      break;
    }
    case VAL_TYPE_REALTIME_END: {

      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;

      if ( uuid_run != UUID_RUN || uuid_send_realtime != UUID_SEND_REALTIME ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "received wrong realtime uuid");
	break;
      }

      // time to update the screen
      ds_DynamicArray_swap(&s_stops, &s_stops_recv);
      clear();
      draw();
      Stops_clear(&s_stops_recv);
      break;
    }
    case VAL_TYPE_STATE: {

      switch(dict_find(iterator, MESSAGE_KEY_STATE)->value->uint32){
	case VAL_STATE_LOADING:{
	  status_bar_layer_set_colors(s_status_bar, BLO, FLO);
	  break;
        }
	case VAL_STATE_LOADED:{
	  status_bar_layer_set_colors(s_status_bar, BOK, FOK);
	  break;
        }
	case VAL_STATE_LOADED_GEOERROR:{
	  status_bar_layer_set_colors(s_status_bar, BNG, FNG);
	  break;
        }
	case VAL_STATE_LOADED_ERROR:{
	  status_bar_layer_set_colors(s_status_bar, BKO, FKO);
	  break;
        }
      }
      break;
    }
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %i - %s", reason, pebble_translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %i - %s", reason, pebble_translate_error(reason));
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
	// Create main window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	// Show the window on the watch with animated=true
	window_stack_push(s_main_window, true);

	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	// Open AppMessage
	const int inbox_size = 128;
	const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}

static void other ( ) {

  ++s_displayed_stop_index;

  if ( s_displayed_stop_index >= s_stops.length ) {
    // cycle
    s_displayed_stop_index = 0 ;
  }

  clear();
  draw();

}

static void handle_error ( const char* title , const char* message ) {

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
