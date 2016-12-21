#include <pebble.h>
#include "u/copy.h"

char* u_copy_string(const char* src){
	char *dest = (char*)malloc((strlen(src)+1)*sizeof(char));
	strcpy(dest, src);
	return dest;
}