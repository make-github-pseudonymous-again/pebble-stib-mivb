var _UUID = -1 ;

function Lock ( ) {
	this._UUID = ++_UUID ;
	this._LOCK = false ;
}

Lock.prototype.acquire = function ( ) {
	if ( this._LOCK ) {
		console.log( 'could not acquire lock', this._UUID ) ;
		return false ;
	}
	console.log( 'acquired lock', this._UUID ) ;
	this._LOCK = true ;
	return true ;
} ;

Lock.prototype.release = function ( ) {
	console.log( 'released lock', this._UUID ) ;
	this._LOCK = false ;
} ;

function create ( ) {
	return new Lock() ;
}

module.exports.create = create ;