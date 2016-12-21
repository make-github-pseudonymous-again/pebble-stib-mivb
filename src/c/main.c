#include <pebble.h>

static const int TKO = 60000 ;

// colors

static const int BOK = 0x55AA55 ;
static const int FOK = 0xFFFFFF ;
static const int BLO = 0xFFFF55 ;
static const int FLO = 0x000000 ;
static const int BKO = 0xFF0055 ;
static const int FKO = 0xFFFFFF ;
static const int BNG = 0xFFAA00 ;
static const int FNG = 0xFFFFFF ;
static const int BG = 0xFFFFFF ;

// https://stackoverflow.com/a/21172222/1582182
char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

// Growing array from https://stackoverflow.com/a/3536261/1582182

typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;

void initArray(Array *a, size_t initialSize) {
  a->array = (int *)malloc(initialSize * sizeof(int));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, int element) {
  if (a->used == a->size) {
    a->size *= 2;
    a->array = (int *)realloc(a->array, a->size * sizeof(int));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

typedef struct RealtimePacket {
	int stop_id;
	char* stop_name;
	char* line_name;
	char* destination_name;
	int foreground_color;
	int background_color;
	int utc;
} RealtimePacket;

static Window *s_main_window;
static TextLayer *s_time_layer;
static uint32_t UUID_RUN;
static uint32_t UUID_SEND_REALTIME;

static void _update_time(struct tm *tick_time) {
	
	// Write the current hours and minutes into a buffer
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
}

static void update_time() {

	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	// Update with that tm structure
	_update_time(tick_time);
	
}

static void main_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_time_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

	// Improve the layout to be more like a watchface
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	_update_time(tick_time);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  switch (dict_find(iterator, MESSAGE_KEY_TYPE)->value->uint32) {
    case VAL_TYPE_REALTIME: {
      if (!stations) {
        // New close stations.
        uint32_t number = dict_find(iterator, KEY_NUMBER_OF_STATIONS)->value->uint32;
        stations = Stations_new(number);
      } else {
        // If the stations array has already been allocated.
        // free_stations(Stations);
      }
		
		const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
		const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
		const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;
		
		if ( uuid_run > UUID_RUN || uuid_send_realtime > UUID_SEND_REALTIME ) {
			UUID_RUN = uuid_run;
			UUID_SEND_REALTIME = uuid_send_realtime;
			list_clear(realtimes);
		}
		
	
		
	  // space for Realtime object must be dynamically allocated
	  // space for cstring object must be dynamically allocated
	  (Realtime) {
		.stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32,
		.stop_name = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_NAME)->value->cstring,
		.line_name = dict_find(iterator, MESSAGE_KEY_REALTIME_LINE_NAME)->value->cstring,
		.destination_name = dict_find(iterator, MESSAGE_KEY_REALTIME_DESTINATION_NAME)->value->cstring,
		.foreground_color = dict_find(iterator, MESSAGE_KEY_REALTIME_FOREGROUND_COLOR)->value->uint32,
		.background_color = dict_find(iterator, MESSAGE_KEY_REALTIME_BACKGROUND_COLOR)->value->uint32,
		.utc = dict_find(iterator, MESSAGE_KEY_REALTIME_UTC)->value->uint32,
	  };

      list_add(realtimes, realtime);
      break;
    }
    case RESPONSE_UPDATED_STATIONS:
    case RESPONSE_UPDATED_LOCATION: {
      DEBUG("Updating the location.");

      stations->update(stations, Station_new (
        dict_find(iterator, KEY_NAME)->value->cstring,
        dict_find(iterator, KEY_PARKINGS)->value->uint32,
        dict_find(iterator, KEY_FREE_BIKE)->value->uint32,
        dict_find(iterator, KEY_DISTANCE)->value->uint32,
        dict_find(iterator, KEY_ANGLE)->value->uint32
      ));

      // TODO No update if the current station is no longer in the array.

      break;
    }
    case RESPONSE_ADD_STATIONS: {
      DEBUG("Adding stations.");

      stations->add(stations, Station_new (
        dict_find(iterator, KEY_NAME)->value->cstring,
        dict_find(iterator, KEY_PARKINGS)->value->uint32,
        dict_find(iterator, KEY_FREE_BIKE)->value->uint32,
        dict_find(iterator, KEY_DISTANCE)->value->uint32,
        dict_find(iterator, KEY_ANGLE)->value->uint32
      ));
      break;
    }
    case RESPONSE_END: {
      win_main_update ();

      /* Reenable the tick timer to fetch new location. */
      tick_timer_service_subscribe(SECOND_UNIT, second_handler);
      break;
    }
    default: {
      break;
    }
}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %i - %s", reason, translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed: %i - %s", reason, translate_error(reason));
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
	
	// Show current time
	update_time();
	
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


static void ad ( Layer* f ) {
	WIDGETS.push(f);
	MAIN.add(f);
}

static void rm ( Layer* f ) {
	f.text('');
	MAIN.remove(f);
}

static void clear ( ) {
	var len = WIDGETS.length ;
	for ( var i = 0 ; i < len ; ++i ) rm(WIDGETS[i]);
	WIDGETS = [] ;
}

static void _display ( quiet ) {
	
	console.log('display');
	
	clear();
	
	STOP_ID = DATA.stops[STOP_INDEX].id ;
	
	FSTOP.text( DATA.stops[STOP_INDEX].name );
	ad(FSTOP);
	
	if ( DATA.stops[STOP_INDEX].realtime.error ) {
		FMESSAGE.text( DATA.stops[STOP_INDEX].realtime.message );
		ad(FMESSAGE);
		return ;
	}
	
	var n = DATA.stops[STOP_INDEX].realtime.results.length;
	
	var k = 0 ;
	
	for ( var i = 0 ; i < n ; ++i ) {
	
		var next = DATA.stops[STOP_INDEX].realtime.results[i] ;
		
		var _when = when( next ) ;
		
		if ( _when === null ) continue ;

		var offset = k*35 ;
		++k ;

		var fnumber = new UI.Text({
		 position: new Vector2(LEFT, 30+offset),
		 size: new Vector2(32, 32),
		 font: 'gothic-24-bold',
		 textAlign: 'center',
		 textOverflow: 'fill',
		 text: next.line,
			// use GColorFromHEX(next.fgcolor)
		 color: next.fgcolor,
		 backgroundColor: next.bgcolor
		});

		var fline = new UI.Text({
		 position: new Vector2(LEFT+37, 30+offset),
		 size: new Vector2(W-91, 20),
		 font: 'gothic-24-bold',
		 color: '#000000' ,
		 textAlign: 'left',
		 textOverflow: 'ellipsis' ,
		 text: next.destination
		});

		var fminutes = new UI.Text({
		 position: new Vector2(LEFT+W-54,30+offset),
		 size: new Vector2(22, 20),
		 font: 'gothic-24-bold',
		 text: _when.minutes ,
		 color: _when.color ,
		 backgroundColor: 'none',
		 textAlign: 'center',
		 textOverflow: 'fill'
		});	

		ad(fnumber);
		ad(fline);
		ad(fminutes);

		if ( !quiet && k === 1 && _when.minutes === 0 ) Vibe.vibrate('double');
		
	}
	
	if ( k === 0 ) {
		FMESSAGE.text( 'nothing right now' );
		ad(FMESSAGE);
	}
	
}

static void other ( ) {
	++STOP_INDEX ;
	if ( STOP_INDEX >= DATA.stops.length ) STOP_INDEX = 0 ;
	_display( true ) ;
}


static void handle_error ( title , message ) {
	console.log( 'handle_error:', title, message ) ;
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


static Minutes when ( int expected_arrival ) {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	int now = 0; // TODO find seconds since 1970 0 0 0 utc
	// the +5 is to account for data transmission
	// and code execution between data retrieval and display
	int seconds = ( expected_arrival - now ) / 1000 + 5 ;
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, expected_arrival , now , seconds);
	
	if ( seconds < -600 ) {
		// 10 minutes ago can be safely ignored
		return null ;
	}
	else if  ( seconds < 0 ) {
		// probably already gone
		return Minutes { color : 0xFF0055 , minutes : 0 } ;
	}
	else {
		return Minutes { color : 0x555555 , minutes : seconds / 60 } ;
	}
}

var FSTOP = new UI.Text({
 position: new Vector2(25, 0),
 size: new Vector2(W - 50, 20),
 font: 'gothic-24-bold',
 backgroundColor: 'none',
 color: '#000000' ,
 textAlign: 'center',
 textOverflow: 'ellipsis'
});

var FMESSAGE = new UI.Text({
 position: new Vector2(LEFT, 30),
 size: new Vector2(W - 34, H - 30),
 font: 'gothic-24-bold',
 color: BKO ,
 textAlign: 'center',
 textOverflow: 'wrap'
});

var SIZE = MAIN.size() ;
var W = SIZE.x ;
var H = SIZE.y ;
var LEFT = 15;



// display

var WIDGETS = [];

var MAIN = new UI.Window({
	scrollable: true,
 	backgroundColor: BG,
	status: {
		separator : 'none',
		color: FKO,
		backgroundColor: BKO
	}
});




MAIN.show();


static void loadsuccess (cb, fg, bg, quiet) {
	MAIN.status('color', fg);
	MAIN.status('backgroundColor', bg);
	TIMESTAMP = Date.now();
	TIMEOUT = setTimeout( load , POLLRATE ) ;
}

static void loading ( ) {
	MAIN.status('color', FLO ) ;
	MAIN.status('backgroundColor', BLO ) ;
}

static void bindload ( ) {
	
	unbind();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "bindload");

	MAIN.on('click', 'select', function(e) { console.log('click'); load( null , true ) ; } ) ;
	MAIN.on('longClick', 'select', function(e) { console.log('longClick'); load( null , true ) ; } ) ;
	
	release();
	
}

static void bindnav ( ) {
	
	unbind();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "bindnav");
	
	MAIN.on('click', 'select', function(e) { console.log('click'); other() ; } ) ;
	MAIN.on('longClick', 'select', function(e) { console.log('longClick'); load( null , true ) ; } ) ;

	MAIN.on('hide', function(e){
		console.log('hide'); 
		if ( TIMEOUT !== null ) {
			clearTimeout(TIMEOUT);
			TIMEOUT = null ;
		}
		freeze();
	});

	MAIN.on('show', function(e){
		console.log('show'); 
		load( null , true );
	});
	
	release();
}

static void unbind ( ) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "unbind");	
	try { MAIN.off('click') ; }     catch ( e ) { }
	try { MAIN.off('longClick') ; } catch ( e ) { }
	try { MAIN.off('hide') ; }      catch ( e ) { }
	try { MAIN.off('show') ; }      catch ( e ) { }	
}


