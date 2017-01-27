#include <pebble.h>
#include "when.h"

const int WHEN_GONE_THRESHOLD = -300;

// each string below must have size <= WHEN_BUFFER_SIZE - 1 (see when.h)
const char *WHEN_IN_MORE_THAN_100_MINUTES = ">99";
const char *WHEN_GONE = ":(";
const char *WHEN_NOW = "0";

GColor when (bool quiet, char* buffer, const time_t now, const time_t expected_arrival) {
  
	// The +30 is to account for data transmission
	// and code execution between data retrieval and display.
	// Should probably send the time of retrieval to avoid
	// data transmission delay.
  // Note that without +30, redrawing the screen just
  // after loading would remove 1 minute from the counters.
	const time_t seconds = expected_arrival - now + 30 ;

	APP_LOG(APP_LOG_LEVEL_DEBUG, "%ld %ld %ld", expected_arrival , now , seconds);

	if ( seconds < WHEN_GONE_THRESHOLD ) {
    if (!quiet) vibes_double_pulse();
		strcpy(buffer, WHEN_GONE);
		return GColorPurple ;
	}
	else if  ( seconds < 0 ) {
    if (!quiet) vibes_double_pulse();
		strcpy(buffer, WHEN_NOW);
		return GColorFolly ;
	}
	else if  ( seconds < 60 ) {
    if (!quiet) vibes_double_pulse();
		strcpy(buffer, WHEN_NOW);
		return GColorDarkGray ;
	}
	else if  ( seconds >= 6000 ) {
		strcpy(buffer, WHEN_IN_MORE_THAN_100_MINUTES);
		return GColorDarkGray ;
	}
	else { // 60 <= seconds < 6000
		snprintf(buffer, WHEN_BUFFER_SIZE, "%ld", seconds / 60);
		return GColorDarkGray ;
	}

}
