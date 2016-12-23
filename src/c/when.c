#include <pebble.h>
#include "when.h"

static GColor when ( char* buffer, time_t now, time_t expected_arrival ) {
	// The +5 is to account for data transmission
	// and code execution between data retrieval and display.
	// Should probably send the time of retrieval to avoid
	// data transmission delay.
	time_t seconds = expected_arrival - now + 5 ;

	APP_LOG(APP_LOG_LEVEL_DEBUG, expected_arrival , now , seconds);

	if ( seconds < WHEN_GONE_THRESHOLD ) {
		strcpy(buffer, WHEN_GONE);
		return GColorPurple ;
	}
	else if  ( seconds < 0 ) {
		strcpy(buffer, WHEN_NOW);
		return GColorFolly ;
	}
	else if  ( seconds >= 6000 ) {
		strcpy(buffer, WHEN_IN_MORE_THAN_100_MINUTES);
		return GColorDarkGray ;
	}
	else {
		snprintf(buffer, MINUTES_BUFFER_SIZE, "%d", seconds / 60);
		return GColorDarkGray ;
	}

}
