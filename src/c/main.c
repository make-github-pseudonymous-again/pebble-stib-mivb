#include <pebble.h>
#include "minutes.h"
#include "realtime.h"
#include "ds/bisect.h"
#include "ds/dynamicarray.h"
#include "pebble/translate_error.h"

// CONSTANTS

static const uint32_t VAL_TYPE_STATE = 0 ;
static const uint32_t VAL_TYPE_REALTIME = 1 ;
static const uint32_t VAL_TYPE_REALTIME_END = 2 ;

// KO timeout
static const int TKO = 60000 ;

// colors
static const GColor BOK = GColorFromHex(0x55AA55) ;
static const GColor FOK = GColorFromHex(0xFFFFFF) ;
static const GColor BLO = GColorFromHex(0xFFFF55) ;
static const GColor FLO = GColorFromHex(0x000000) ;
static const GColor BKO = GColorFromHex(0xFF0055) ;
static const GColor FKO = GColorFromHex(0xFFFFFF) ;
static const GColor BNG = GColorFromHex(0xFFAA00) ;
static const GColor FNG = GColorFromHex(0xFFFFFF) ;
static const GColor  BG = GColorFromHex(0xFFFFFF) ;

// font
static const char *FONT = FONT_KEY_GOTHIC_24_BOLD;

// GLOBALS
static uint32_t s_last_seen_stop_id;
// why use pointers if they are globals???
static ds_DynamicArray *s_realtimes_index;
static ds_DynamicArray *s_realtimes;

static ds_DynamicArray *s_stops_name;
static char s_minutes_buffer[2][4];
static TextLayer* s_line_number_layer[2];	
static TextLayer* s_destination_name_layer[2];
static TextLayer* s_minutes_layer[2];
static ds_DynamicArray *s_stops_id;
static uint32_t s_displayed_stop_id = 0;
static size_t s_displayed_stop_index = 0;

static Window *s_main_window;
static StatusBarLayer *s_status_bar;
static TextLayer *s_info_layer;
static TextLayer *s_stop_name_layer;
static TextLayer *s_message_layer;
static uint32_t s_uuid_run;
static uint32_t s_uuid_send_realtime;
static GRect s_bounds;
static GSize s_size ;
static int16_t s_w ;
static int16_t s_h ;


static void ad ( Layer* layer ) {
	Layer *window_layer = window_get_root_layer(s_main_window);
	layer_add_child(window_layer, layer);
}

static void clear ( ) {
	Layer *window_layer = window_get_root_layer(s_main_window);
	layer_remove_child_layers(window_layer);
}

static void update_display_from_time(const time_t now) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "display");
	
	clear();
	
	s_displayed_stop_id = (uint32_t)s_stops_id->data[s_displayed_stop_index] ;
	
	text_layer_set_text(s_stop_name_layer, s_stops_name->data[s_displayed_stop_index]);
	ad(text_layer_get_layer(s_stop_name_layer));
	
	if ( DATA.stops[STOP_INDEX].realtime.error ) {
		FMESSAGE.text( DATA.stops[STOP_INDEX].realtime.message );
		ad(FMESSAGE);
		return ;
	}
	
	var n = DATA.stops[STOP_INDEX].realtime.results.length;
	
	for ( size_t i = 0 ; i < 2 ; ++i ) {
	
		var next = DATA.stops[STOP_INDEX].realtime.results[i] ;
		
		var _when = when( next ) ;
		
		if ( _when === null ) continue ;

		int16_t offset = i*35 ;
		
		// TODO fix
		
		number_layer = s_number_layer[i] ;
		fline = s_destination_layer[i] ;
		fminutes = s_minutes_layer[i] ;

		fnumber = text_layer_create(GRect(s_left, 30+offset, 32, 32));
		text_layer_set_font(fnumber, fonts_get_system_font(FONT));
		text_layer_set_text_alignment(fnumber, GTextAlignmentCenter);
		text_layer_set_overflow_mode(fnumber, GTextOverflowModeFill);
		text_layer_set_text(fnumber, realtime->line_number);
		text_layer_set_text_color(fnumber, GColorFromHex(realtime->foreground_color));
		text_layer_set_background_color(fnumber, GColorFromHex(realtime->background_color));
		
		fline = text_layer_create(GRect(s_left+37, 30+offset, s_w-91, 20));
		text_layer_set_font(fnumber, fonts_get_system_font(FONT));
		text_layer_set_text_alignment(fnumber, GTextAlignmentLeft);
		text_layer_set_overflow_mode(fnumber, GTextOverflowModeTrailingEllipsis);
		text_layer_set_text(fnumber, realtime->destination_name);
		text_layer_set_text_color(fnumber, GColorBlack);

		fminutes = text_layer_create(GRect(s_left+s_w-54, 30+offset, 22, 20));
		text_layer_set_font(fnumber, fonts_get_system_font(FONT));
		text_layer_set_text_alignment(fnumber, GTextAlignmentCenter);
		text_layer_set_overflow_mode(fnumber, GTextOverflowModeFill);
		text_layer_set_text(fnumber, realtime->line_number);
		text_layer_set_text_color(fnumber, GColorFromHex(realtime->foreground_color));

		var fminutes = new UI.Text({
		 text: _when.minutes ,
		 color: _when.color ,
		});	

		layer_add_child(window_layer, fnumber);
		layer_add_child(window_layer, fline);
		layer_add_child(window_layer, fminutes);

		if ( !quiet && k === 1 && _when.minutes === 0 ) Vibe.vibrate('double');
		
	}
	
	if ( k === 0 ) {
		FMESSAGE.text( 'nothing right now' );
		ad(FMESSAGE);
	}
	

}

static void update_display_from_tm(struct tm *tick_time) {
	const time_t now = mktime(tick_time);
	update_display_from_time(now);
}

static void update_display() {
	time_t now = time(NULL);
	update_display_from_time(tick_time);
}

static int16_t s_scroll = 0;
static const int16_t s_lineheight = 16;

static void scroll_up(){
	
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
	s_left = 15;
	
	// Create the StatusBarLayer
	s_status_bar = status_bar_layer_create();
	status_bar_layer_set_colors(s_status_bar, BKO, FKO);
	status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);
	layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
	
	// Stop name text layer
	var FSTOP = new UI.Text({
	 position: new Vector2(25, 0),
	 size: new Vector2(W - 50, 20),
	 font: 'gothic-24-bold',
	 backgroundColor: 'none',
	 color: '#000000' ,
	 textAlign: 'center',
	 textOverflow: 'ellipsis'
	});

	// Message text layer
	var FMESSAGE = new UI.Text({
	 position: new Vector2(LEFT, 30),
	 size: new Vector2(W - 34, H - 30),
	 font: 'gothic-24-bold',
	 color: BKO ,
	 textAlign: 'center',
	 textOverflow: 'wrap'
	});
	
	// Update display with cached information
	update_display();

	// Register click events
	window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

	// display

	var MAIN = new UI.Window({
		scrollable: true,
		window_set_background_color(window, BG);

	});
}

static void main_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload");
	status_bar_layer_destroy(s_status_bar);	
	// TODO cache data on the watch
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	_update_display(tick_time);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	
  switch (dict_find(iterator, MESSAGE_KEY_TYPE)->value->uint32) {
    case VAL_TYPE_REALTIME: {
		
		const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
		const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
		//const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;
		
		if ( uuid_run < UUID_RUN || ( uuid_run == UUID_RUN && uuid_send_realtime < UUID_SEND_REALTIME ) ) {
			// this should  never happen
			APP_LOG(APP_LOG_LEVEL_ERROR, "skipping message");
			return;
		}
		
		if ( uuid_run > UUID_RUN || uuid_send_realtime > UUID_SEND_REALTIME ) {
			// we are receiving new realtime data, drop old
			UUID_RUN = uuid_run;
			UUID_SEND_REALTIME = uuid_send_realtime;
			const size_t n = REALTIMES->length;
			for (size_t i = 0 ; i < n ; ++i){
				const Realtime *realtime = REALTIMES->data[i];
				Realtime_free(realtime);
			}
			ds_DynamicArray_free(REALTIMES);
		}
		
		const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
		const char *stop_name = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_NAME)->value->cstring;
		const char *line_name = dict_find(iterator, MESSAGE_KEY_REALTIME_LINE_NAME)->value->cstring;
		const char *destination_name = dict_find(iterator, MESSAGE_KEY_REALTIME_DESTINATION_NAME)->value->cstring;
		const uint32_t foreground_color = dict_find(iterator, MESSAGE_KEY_REALTIME_FOREGROUND_COLOR)->value->uint32;
		const uint32_t background_color = dict_find(iterator, MESSAGE_KEY_REALTIME_BACKGROUND_COLOR)->value->uint32;
		const uint32_t utc = dict_find(iterator, MESSAGE_KEY_REALTIME_UTC)->value->uint32;
		
		Realtime *realtime = Realtime_create(
			stop_id,
			stop_name,
			line_name,
			destination_name,
			foreground_color,
			background_color,
			utc
		);

		ds_DynamicArray_push(REALTIMES, realtime);
		break;
    }
    case VAL_TYPE_STATE: {
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
	++STOP_INDEX ;
	if ( STOP_INDEX >= DATA.stops.length ) STOP_INDEX = 0 ;
	_display( true ) ;
}


static void handle_error ( const char* title , const char* message ) {
	
	APP_LOG(APP_LOG_LEVEL_ERROR, "handle_error:", title, message);
	
	if ( Date.now() - TIMESTAMP < TKO ) {
		bindnav();
		MAIN.status('color', FOK);
		MAIN.status('backgroundColor', BOK);
		TIMEOUT = setTimeout( load , POLLRATE ) ;
		return ;
	}
	MAIN.status('color', FKO);
	MAIN.status('backgroundColor', BKO);
	clear();
	FSTOP.text( title );
	FMESSAGE.text( message );
	ad(FSTOP);
	ad(FMESSAGE);
	bindload();
}

static void loadsuccess (cb, fg, bg, quiet) {
	status_bar_layer_set_colors(s_status_bar, bg, fg);
	TIMESTAMP = Date.now();
	TIMEOUT = setTimeout( load , POLLRATE ) ;
}

static void loading ( ) {
	status_bar_layer_set_colors(s_status_bar, BLO, FLO);
}
