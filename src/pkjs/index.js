var UUID_RUN = Date.now() / 1000 ;
var UUID_SEND_DATA = -1 ;

var VAL_TYPE_STATE = 0 ;
var VAL_TYPE_REALTIME = 1 ;
var VAL_STATE_LOADED_GEOERROR = 1 ;
var VAL_STATE_LOADED = 2 ;

// LOCKING

var _LOCK = false ;

function lock ( ) {
	if ( _LOCK ) {
		console.log( 'could not acquire lock' ) ;
		return false ;
	}
	console.log( 'acquired lock' ) ;
	_LOCK = true ;
	return true ;
}

function release ( ) {
	console.log( 'released lock' ) ;
	_LOCK = false ;
}

// GLOBALS

var GEOID = null ;
var GEOERROR = 'GEOLOCATION NOT STARTED' ;
var LAT = null ;
var LON = null ;
var DATA = null ;
var TIMESTAMP = 0 ;
var STOP_ID = null ;
var ERROR = null ;
var MAX_REQUESTS = 10 ;
var NCLOSEST = 10 ;
var POLLRATE = 30000 ;
var NAVIGATOR_GEOLOCATION_OPTS = {
	maximumAge:0,
	timeout:5000,
	enableHighAccuracy:true
};

var TIMEOUT = null;

// (UN)FREEZE

var DEFAULT_STATE = {
	LAT : LAT ,
	LON : LON ,
	DATA : DATA ,
	STOP_ID : STOP_ID
} ;

function freeze ( ) {
	console.log('freeze');
	var state = {
		LAT : LAT ,
		LON : LON ,
		DATA : DATA ,
		STOP_ID : STOP_ID
	} ;
	localStorage.setItem('state', state);
}

function unfreeze ( ) {
	console.log('unfreeze');
	var state = localStorage.getItem('state') || DEFAULT_STATE ;
	LAT = state.LAT ;
	LON = state.LON ;
	DATA = state.DATA ;
	STOP_ID = state.STOP_ID ;
}

// REALTIME

function api ( lat , lon ) {
	var n = NCLOSEST ;
	var m = MAX_REQUESTS ;
	return 'https://stib-mivb-api.herokuapp.com/realtime/nclosest/' + n + '/' + lat + '/' + lon + '?max_requests=' + m ;
}

function handle_error () {
	release();
}

function loadfail (cb) {
	return function (event) {
		handle_error('API failed ' + event.status , event.response.message ) ;
		if ( cb ) cb() ;
	} ;
}

function loadsuccess (cb, geoerror, quiet) {
	return function ( event ) {
		release();
		ERROR = null ;
		DATA = event.response ;
		freeze();
		sendState( geoerror ? KEY_STATE_LOADED_GEOERROR : KEY_STATE_LOADED);
		sendData();
		TIMESTAMP = Date.now();
		TIMEOUT = setTimeout( load , POLLRATE ) ;
		if ( cb ) cb() ;
	} ;
}

function load ( cb, quiet ) {
	
	console.log( 'try load' ) ;
	
	if ( !lock() ) return ;
	
	console.log( 'load' ) ;
	
	if ( TIMEOUT !== null ) {
		clearTimeout(TIMEOUT);
		TIMEOUT = null ;
	}
	
	sendState('LOADING');
	
	if ( LAT === null || LON === null  ) {
		return handle_error('GEOERROR', GEOERROR);
	}

	// TODO timeout and abort long requests
	// Create the request
	var request = new XMLHttpRequest();
	
	//request.addEventListener("progress", updateProgress);
	request.addEventListener('load', loadsuccess( cb, GEOERROR, quiet ));
	request.addEventListener('error', loadfail( cb ));
	//request.addEventListener("abort", transferCanceled);

	// Send the request
	var url = api(LAT, LON);
	request.open('GET', url);
	request.setRequestHeader('Content-Type', 'application/json');
	request.responseType = 'json' ;
	request.send();
}

// GEOLOCATION

function geosuccess ( position ) {
	LAT = position.coords.latitude;
	LON = position.coords.longitude;
	freeze();
	console.log( 'geosuccess', LAT, LON ) ;
	var failed_before = GEOERROR !== null ;
	GEOERROR = null ;
	if ( TIMEOUT === null || failed_before ) load( null );
}

function geofail(){
	console.log( 'geofail' ) ;
	GEOERROR = 'could not load geolocation :(';
	if ( TIMEOUT === null ) load( null );
}

function geostart(){
	console.log( 'geostart' );
	
	if(navigator && navigator.geolocation){
		
		if ( GEOID !== null ) {
			navigator.geolocation.clearWatch( GEOID ) ;
			GEOID = null ;
		}
		var opts = NAVIGATOR_GEOLOCATION_OPTS;
		GEOID = navigator.geolocation.watchPosition(geosuccess, geofail, opts);
		GEOERROR = 'GEOLOCATION LOADING...' ;
	}
	else{
		GEOERROR = 'navigator not enabled :(' ;
	}
}

//function geostop(){
//	console.log( 'geostop' );
//	if ( navigator && navigator.geolocation && GEOID !== null ) {
//		navigator.geolocation.clearWatch( GEOID ) ;
//		GEOID = null ;
//	}
//	GEOERROR = 'GEOLOCATION STOPPED' ;
//}

// COMMUNICATION

function utc ( string ) {
	// 2016-10-06T12:12:41+02:00
	var _year = string.substring( 0 , 4 ) ;
	var _month = string.substring( 5 , 7 ) ;
	var _day = string.substring( 8 , 10 ) ;
	var _hour = string.substring( 11 , 13 ) ;
	var _minutes = string.substring( 14 , 16 ) ;
	var _seconds = string.substring( 17 , 19 ) ;
	var _tzs = string[19] ;
	var _tzh = string.substring( 20 , 22 ) ;
	var _tzm = string.substring( 23 , 25 ) ;
	
	var year = parseInt( _year , 10 ) ;
	var month = parseInt( _month , 10 ) ;
	var day = parseInt( _day , 10 ) ;
	var hour = parseInt( _hour , 10 ) ;
	var minutes = parseInt( _minutes , 10 ) ;
	var seconds = parseInt( _seconds , 10 ) ;
	var tzs = _tzs === '+' ? 1 : -1 ;
	var tzh = parseInt( _tzh , 10 ) ;
	var tzm = parseInt( _tzm , 10 ) ;
	
	// account for timezone
	hour -= tzs * tzh ;
	minutes -= tzs * tzm ;
	
	// * hour or minutes can be negative, handled correctly by UTC
	// * month must be between 0 and 11 /!\
	// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/UTC
	return Date.UTC(year, month - 1, day, hour, minutes, seconds);
}

function utc32 ( string ) {
	return utc( string ) / 1000 ;
}

function sendState ( state ) {
	Pebble.sendAppMessage( {
		'KEY_COMMUNICATION': KEY_COMMUNICATION_STATE,
		'KEY_STATE': state
	},
	function(e) {
		console.log(state + ' state sent to Pebble successfully!');
	},
	function(e) {
		console.log('Error sending ' + state + ' state to Pebble!');
	});
}

function sendData ( ) {
	++UUID_SEND_DATA;
	var stops = DATA.stops ;
	var n = stops.length ;
	var uuid_send_data_item = -1;
	for ( var i = 0 ; i < n ; ++i ) {
		var stop = stops[i] ;
		var id = parseInt(stop.id, 10);
		var name = stop.name ;
		var results = stop.realtime.results ;
		var m = results.length ;
		for ( var j = 0 ; j < m ; ++j, ++id ) {
			var result = results[j] ;
			// TODO optimize by sending static data only once (colors)
			// and send ids instead of names (line, destination, etc.)
			var payload = {
				'TYPE': VAL_TYPE_REALTIME, // 8 bits -> int32
				'UUID_RUN': UUID_RUN, // int -> int32
				'UUID_SEND_DATA': UUID_SEND_DATA, // int -> int32
				'UUID_SEND_DATA_ITEM': ++uuid_send_data_item, // int -> int32
				'REALTIME_STOP_ID': id, // int -> int32
				'REALTIME_STOP_NAME': name, // string -> cstring
				'REALTIME_LINE_NAME': result.line, // string -> cstring
				'REALTIME_DESTINATION_NAME': result.destination, // string -> cstring
				'REALTIME_FGCOLOR': parseInt(result.fgcolor, 16), // 24 bits -> int32
				'REALTIME_BGCOLOR': parseInt(result.bgcolor, 16), // 24 bits -> int32
				// utc32 will work till Date 2038-01-19T03:14:07.000Z
				// i.e., new Date((Math.pow(2,31)-1)*1000);
				// but then we'll probably have quantum/ADN watches
				'REALTIME_UTC': utc32(result.when), // int -> int32
			} ;
		}	
	}
}

// MAIN

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {  
	unfreeze();
	if ( LAT !== null && LON !== null ) load( geostart );
	else geostart();
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
  }                     
);