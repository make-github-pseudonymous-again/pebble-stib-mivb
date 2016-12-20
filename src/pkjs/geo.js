var state = require('./state');

var NAVIGATOR_GEOLOCATION_OPTS = {
	maximumAge:0,
	timeout:5000,
	enableHighAccuracy:true
} ;

function Geo ( cb ) {
	this.id = null ;
	this.error = 'GEOLOCATION NOT STARTED' ;
	this.lat = null ;
	this.lon = null ;
	this.cb = cb ;
}

Geo.prototype.onsuccess = function ( position ) {
	this.lat = position.coords.latitude;
	this.lon = position.coords.longitude;
	state.freeze();
	console.log( 'geosuccess', this.lat, this.lon ) ;
	var failed_just_before = this.error !== null ;
	this.error = null ;
	this.cb(failed_just_before);
} ;

Geo.prototype.onfailure = function () {
	console.log( 'geofail' ) ;
	this.error = 'could not load geolocation :(';
	this.cb();
} ;

Geo.prototype.start = function () {
	console.log( 'geostart' );
	
	if (navigator && navigator.geolocation) {
		
		if ( this.id !== null ) {
			navigator.geolocation.clearWatch( this.id ) ;
			this.id = null ;
		}
		var opts = NAVIGATOR_GEOLOCATION_OPTS;
		var success = this.onsuccess.bind(this) ;
		var failure = this.onfailure.bind(this) ;
		this.id = navigator.geolocation.watchPosition(success, failure, opts);
		this.error = 'GEOLOCATION LOADING...' ;
	}
	else{
		this.error = 'navigator not enabled :(' ;
	}
} ;

Geo.prototype.stop = function (){
	console.log( 'geostop' );
	if ( navigator && navigator.geolocation && this.id !== null ) {
		navigator.geolocation.clearWatch( this.id ) ;
		this.id = null ;
	}
	this.error = 'GEOLOCATION STOPPED' ;
} ;

function create (cb) {
	return new Geo(cb);
}

module.exports.create = create ;