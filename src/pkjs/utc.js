
function get ( string ) {
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

function get32 ( string ) {
	return ( get( string ) / 1000 ) | 0 ;
}

module.exports.get = get ;
module.exports.get32 = get32 ;