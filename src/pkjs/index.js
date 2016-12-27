var api = require('./api');
var geo = require('./geo');
var utc = require('./utc');
var messagequeue = require('./messagequeue');
var lock = require('./lock');
var state = require('./state');

// GLOBALS
var UUID_RUN = -1 ;
var UUID_SEND_REALTIME = -1 ;

var VAL_TYPE_STATE = 0 ;
var VAL_TYPE_REALTIME_STOP = 1 ;
var VAL_TYPE_REALTIME_REALTIME = 2 ;
var VAL_TYPE_REALTIME_END = 3 ;

var VAL_STATE_LOADED_GEOERROR = 0 ;
var VAL_STATE_LOADED = 1 ;
var VAL_STATE_LOADING = 2 ;

var TIMESTAMP = 0 ;
var ERROR = null ;
var POLLRATE = 30000 ;

var TIMEOUT = null;

var LOCK = lock.create();

var QUEUE = messagequeue.create();

var DEFAULT_STATE = {
	lat : null ,
	lon : null ,
	realtime : null ,
	stop_id : null
} ;

var STATE = state.create(DEFAULT_STATE);

var GEO = geo.create( function ( should_update ) {
  STATE.data.lat = GEO.lat ;
  STATE.data.lon = GEO.lon ;
  STATE.freeze();
  if ( should_update || TIMEOUT === null ) load( ) ;
});

// REALTIME

function handle_error () {
  LOCK.release();
  send_state(VAL_STATE_ERROR);
}

function loadfail (cb) {
  return function (event) {
    handle_error('API failed ' + event.status , event.response.message ) ;
    if ( cb ) cb() ;
  } ;
}

function loadsuccess (cb, geoerror, quiet) {
  return function ( event ) {
    LOCK.release();
    ERROR = null ;
    STATE.data.realtime = event.response ;
    STATE.freeze();
    send_state( geoerror ? VAL_STATE_LOADED_GEOERROR : VAL_STATE_LOADED);
    send_realtime();
    TIMESTAMP = Date.now();
    TIMEOUT = setTimeout( load , POLLRATE ) ;
    if ( cb ) cb() ;
  } ;
}

function load ( cb, quiet ) {

	console.log( 'try load' ) ;

	if ( !LOCK.acquire() ) return ;

	console.log( 'load' ) ;

	if ( TIMEOUT !== null ) {
		clearTimeout(TIMEOUT);
		TIMEOUT = null ;
	}

	send_state( VAL_STATE_LOADING );

	if ( STATE.data.lat === null || STATE.data.lon === null  ) {
		return handle_error('GEOERROR', GEO.error);
	}

	// TODO timeout and abort long requests
	// Create the request
	var request = new XMLHttpRequest();

	//request.addEventListener("progress", updateProgress);
	request.addEventListener('load', loadsuccess( cb, GEO.error, quiet ));
	request.addEventListener('error', loadfail( cb ));
	//request.addEventListener("abort", transferCanceled);

	// Send the request
	var url = api.url(STATE.data.lat, STATE.data.lon);
	request.open('GET', url);
	request.setRequestHeader('Content-Type', 'application/json');
	request.responseType = 'json' ;
	request.send();
}



// COMMUNICATION

function send_state ( state ) {
	var packet =  {
		'TYPE': VAL_TYPE_STATE,
		'STATE': state
	} ;
	QUEUE.send( packet );
}

function send_realtime ( ) {
	// QUEUE.withdraw( function( packet ) {
	//	 return packet.TYPE === VAL_TYPE_REALTIME && packet.UUID_SEND_REALTIME <= UUID_SEND_REALTIME
	// });
	++UUID_SEND_REALTIME;
	UUID_SEND_REALTIME|=0;
	var stops = STATE.data.realtime.stops ;
	var n = stops.length ;
	for ( var i = 0 ; i < n ; ++i ) {
		var stop = stops[i] ;
		var id = (parseInt(stop.id, 10))|0;
		var name = stop.name ;
		var error = stop.realtime.error ;
		var message = stop.realtime.message ;

		var stop_packet = {
			'TYPE': VAL_TYPE_REALTIME_STOP, // 8 bits -> uint32
			'UUID_RUN': UUID_RUN, // int -> uint32
			'UUID_SEND_REALTIME': UUID_SEND_REALTIME, // int -> uint32
			'REALTIME_STOP_ID': id, // int -> uint32
			'REALTIME_STOP_NAME': name, // string -> cstring
			'REALTIME_STOP_ERROR': error, // 1 bit -> uint32
			'REALTIME_STOP_MESSAGE': message, // string -> cstring
		} ;
		QUEUE.send( stop_packet );

		var results = stop.realtime.results ;
		var m = results.length ;
		for ( var j = 0 ; j < m ; ++j, ++id ) {
			var result = results[j] ;
			// TODO optimize by sending static data only once (colors)
			// and send ids instead of names (line, destination, etc.)
			var packet = {
				'TYPE': VAL_TYPE_REALTIME_REALTIME, // 8 bits -> uint32
				'UUID_RUN': UUID_RUN, // int -> uint32
				'UUID_SEND_REALTIME': UUID_SEND_REALTIME, // int -> uint32
				'REALTIME_STOP_ID': id, // int -> uint32
				'REALTIME_LINE_NUMBER': result.line, // string -> cstring
				'REALTIME_DESTINATION_NAME': result.destination, // string -> cstring
				'REALTIME_FOREGROUND_COLOR': parseInt(result.fgcolor, 16)|0, // 24 bits -> uint32
				'REALTIME_BACKGROUND_COLOR': parseInt(result.bgcolor, 16)|0, // 24 bits -> uint32
				// utc.get32 will work till Date 2038-01-19T03:14:07.000Z
				// i.e., new Date((Math.pow(2,31)-1)*1000);
				// but then we'll probably have quantum/ADN watches
				'REALTIME_UTC': utc.get32(result.when), // int -> uint32
			} ;
			QUEUE.send( packet );
		}
	}

	var endpacket = {
		'TYPE': VAL_TYPE_REALTIME_END, // 8 bits -> uint32
		'UUID_RUN': UUID_RUN, // int -> uint32
		'UUID_SEND_REALTIME': UUID_SEND_REALTIME, // int -> uint32
	} ;

	QUEUE.send( endpacket ); // means "now is a good time to refresh the screen"
}

// MAIN

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
	UUID_RUN = (Date.now() / 1000)|0 ;
	STATE.thaw();
	if ( STATE.data.lat !== null && STATE.data.lon !== null ) load( GEO.start.bind(GEO) );
	else GEO.start();
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
  }
);
