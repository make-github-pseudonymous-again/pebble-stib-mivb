#include <pebble.h>
#include "minutes.h"

static Minutes when ( time_t now, time_t expected_arrival ) {
	// The +5 is to account for data transmission
	// and code execution between data retrieval and display.
	// Should probably send the time of retrieval to avoid
	// data transmission delay.
	time_t seconds = expected_arrival - now + 5 ;
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, expected_arrival , now , seconds);
	
	if ( seconds < -600 ) {
		// 10 minutes ago can be safely ignored
		return (Minutes) {
			.color = 0 ,
			.minutes = -1
		} ;
	}
	else if  ( seconds < 0 ) {
		// probably already gone so use red color
		return (Minutes) {
			.color = GColorFromHex(0xFF0055) ,
			.minutes = 0
		} ;
	}
	else {
		return (Minutes) {
			.color = GColorFromHex(0x555555) ,
			.minutes = seconds / 60
		} ;
	}
}