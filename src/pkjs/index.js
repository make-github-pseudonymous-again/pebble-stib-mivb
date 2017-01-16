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

var VAL_STATE_LOADING = 0 ;
var VAL_STATE_LOADED = 1 ;
var VAL_STATE_LOADED_GEOERROR = 2 ;
var VAL_STATE_LOADED_ERROR = 3 ;
var VAL_STATE_RECV = 4 ;

var POLL_RATE = 30000;
var POLL_TIMEOUT = 10000;
var POLL_TIMER = null;

var LOCK = lock.create();

var QUEUE = messagequeue.create();

var DEFAULT_STATE = {
	lat : undefined ,
	lon : undefined
} ;

var REALTIME = null;

var STATE = state.create(DEFAULT_STATE);

var GEO = geo.create( function ( should_update ) {
  console.log('geocb');
  STATE.data.lat = GEO.lat ;
  STATE.data.lon = GEO.lon ;
  STATE.freeze();
  if ( should_update || POLL_TIMER === null ) load(null, true);
});

// REALTIME

function handle_error (title, message) {
  console.log('handle_error', title, message);
  LOCK.release();
  send_state(VAL_STATE_LOADED_ERROR);
}

function loadfail (req, cb) {
  return function (event) {
    console.log('loadfail');
    handle_error('[load]', 'failed to connect to service') ;
    if ( cb ) cb() ;
  } ;
}

function loadtimeout (req, cb) {
  return function (event) {
    console.log('loadtimeout');
    handle_error('[load]', 'request timed out') ;
    if ( cb ) cb() ;
  } ;
}

function loadsuccess (req, cb, geoerror, quiet) {
  return function ( event ) {
    console.log('loadsuccess');
    LOCK.release();
    var response = null;
    try{
      response = JSON.parse(req.responseText);
    }
    catch(e){
      handle_error('API failed ' + req.status , 'failed to parse JSON response');
      if ( cb ) cb() ;
      return;
    }
    if (response.error) {
      handle_error('API error ' + req.status , '<'+response.error+'> ' + response.message ) ;
      if ( cb ) cb() ;
      return;
    }
    REALTIME = response;
    send_state(VAL_STATE_RECV);
    send_realtime(quiet);
    send_state( geoerror ? VAL_STATE_LOADED_GEOERROR : VAL_STATE_LOADED);
    POLL_TIMER = setTimeout(load.bind(null, null, false), POLL_RATE) ;
    if ( cb ) cb() ;
  } ;
}

function load ( cb, quiet ) {

	console.log( 'try load' ) ;

	if ( !LOCK.acquire() ) return ;

	console.log( 'load' ) ;

	if ( POLL_TIMER !== null ) {
		clearTimeout(POLL_TIMER);
		POLL_TIMER = null ;
	}

	send_state( VAL_STATE_LOADING );

	if ( STATE.data.lat === undefined || STATE.data.lon === undefined  ) {
		return handle_error('GEOERROR', GEO.error);
	}

	// TODO timeout and abort long requests
	// Create the request
	var request = new XMLHttpRequest();
  
  // Configure timeout
  request.timeout = POLL_TIMEOUT;

	//request.addEventListener("progress", updateProgress);
	request.addEventListener('load', loadsuccess( request, cb, GEO.error, quiet ));
	request.addEventListener('error', loadfail( request, cb ));
	request.addEventListener('timeout', loadtimeout( request, cb ));
	//request.addEventListener("abort", transferCanceled);

	// Send the request
	var url = api.url(STATE.data.lat, STATE.data.lon);
	request.open('GET', url);
	request.setRequestHeader('Content-Type', 'application/json');
	//request.responseType = 'json' ;
  console.log('request', url);
	request.send();
}



// COMMUNICATION

function send_state ( state ) {
  console.log('send_state');
	var packet =  {
		'TYPE': VAL_TYPE_STATE,
		'STATE': state
	} ;
	QUEUE.send( packet );
}

function send_realtime ( quiet ) {
  console.log('send_realtime');
	// QUEUE.withdraw( function( packet ) {
	//	 return packet.TYPE === VAL_TYPE_REALTIME && packet.UUID_SEND_REALTIME <= UUID_SEND_REALTIME
	// });
	++UUID_SEND_REALTIME;
	UUID_SEND_REALTIME|=0;
	var stops = REALTIME.stops ;
	var n = stops.length ;
  if (n < 1) {
    console.log('[send_realtime] nothing to send');
    return;
  }
	for ( var i = 0 ; i < n ; ++i ) {
		var stop = stops[i] ;
		var stop_id = (parseInt(stop.id, 10))|0;
		var stop_name = stop.name ;
		var stop_error = stop.realtime.error ;
		var stop_message = stop.realtime.message ;

		var stop_packet = {
			'TYPE': VAL_TYPE_REALTIME_STOP, // 8 bits -> uint32
			'UUID_RUN': UUID_RUN, // int -> uint32
			'UUID_SEND_REALTIME': UUID_SEND_REALTIME, // int -> uint32
			'REALTIME_STOP_ID': stop_id, // int -> uint32
			'REALTIME_STOP_NAME': stop_name, // string -> cstring
			'REALTIME_STOP_ERROR': stop_error, // 1 bit -> uint32
			'REALTIME_STOP_MESSAGE': stop_message, // string -> cstring
		} ;
		QUEUE.send( stop_packet );

		var results = stop.realtime.results ;
		var m = results.length ;
		for ( var j = 0 ; j < m ; ++j ) {
			var result = results[j] ;
			// TODO optimize by sending static data only once (colors)
			// and send ids instead of names (line, destination, etc.)
			var packet = {
				'TYPE': VAL_TYPE_REALTIME_REALTIME, // 8 bits -> uint32
				'UUID_RUN': UUID_RUN, // int -> uint32
				'UUID_SEND_REALTIME': UUID_SEND_REALTIME, // int -> uint32
				'REALTIME_STOP_ID': stop_id, // int -> uint32
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
    'REALTIME_QUIET': quiet, // bool -> uint8
	} ;

	QUEUE.send( endpacket ); // means "now is a good time to refresh the screen"
}

// MAIN

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
	UUID_RUN = (Date.now() / 1000)|0 ;
	STATE.thaw();
	if ( STATE.data.lat !== undefined && STATE.data.lon !== undefined ) load( GEO.start.bind(GEO), false);
	else GEO.start();
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {
  console.log('Received update request');
  load(null, true);
});
