#include <pebble.h>

var TKO = 60000 ;

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

static Window *s_main_window;
static TextLayer *s_time_layer;

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

static void appmsg_in_dropped(AppMessageResult reason, void *context) {
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


function ad ( f ) {
	WIDGETS.push(f);
	MAIN.add(f);
}

function rm ( f ) {
	f.text('');
	MAIN.remove(f);
}

function clear ( ) {
	var len = WIDGETS.length ;
	for ( var i = 0 ; i < len ; ++i ) rm(WIDGETS[i]);
	WIDGETS = [] ;
}

function _display ( quiet ) {
	
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

function other ( ) {
	++STOP_INDEX ;
	if ( STOP_INDEX >= DATA.stops.length ) STOP_INDEX = 0 ;
	_display( true ) ;
}


function handle_error ( title , message ) {
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


function when ( next ) {
	var expected_arrival =  utc( next.when ) ;
	var now = Date.now() ;
	// the +5 is to account for code execution
	// between data retrieval and display
	var seconds = ( expected_arrival - now ) / 1000 + 5 ;
	
	console.log( expected_arrival , now , seconds ) ;
	
	if ( seconds < -600 ) {
		// 10 minutes ago can be safely ignored
		return null ;
	}
	else if  ( seconds < 0 ) {
		// probably already gone
		return { color : '#FF0055' , minutes : 0 } ;
	}
	else {
		return { color : '#555555' , minutes : seconds / 60 | 0 } ;
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


function loadsuccess (cb, fg, bg, quiet) {
	MAIN.status('color', fg);
	MAIN.status('backgroundColor', bg);
	TIMESTAMP = Date.now();
	TIMEOUT = setTimeout( load , POLLRATE ) ;
}

function loading ( ) {
	MAIN.status('color', FLO ) ;
	MAIN.status('backgroundColor', BLO ) ;
}

function bindload ( ) {
	
	unbind();
	console.log('bindload'); 

	MAIN.on('click', 'select', function(e) { console.log('click'); load( null , true ) ; } ) ;
	MAIN.on('longClick', 'select', function(e) { console.log('longClick'); load( null , true ) ; } ) ;
	
	release();
	
}

function bindnav ( ) {
	
	unbind();
	console.log('bindnav'); 
	
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

function unbind ( ) {
	console.log('unbind'); 	
	try { MAIN.off('click') ; }     catch ( e ) { }
	try { MAIN.off('longClick') ; } catch ( e ) { }
	try { MAIN.off('hide') ; }      catch ( e ) { }
	try { MAIN.off('show') ; }      catch ( e ) { }	
}


// colors

var BOK = '#55AA55' ;
var FOK = '#FFFFFF' ;
var BLO = '#FFFF55' ;
var FLO = '#000000' ;
var BKO = '#FF0055' ;
var FKO = '#FFFFFF' ;
var BNG = '#FFAA00' ;
var FNG = '#FFFFFF' ;
var BG = '#FFFFFF' ;
