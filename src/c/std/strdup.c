#include <pebble.h>
#include "strdup.h"

char* strdup(const char* src){
	char *dest = malloc((strlen(src)+1)*sizeof(char));
	strcpy(dest, src);
	return dest;
}