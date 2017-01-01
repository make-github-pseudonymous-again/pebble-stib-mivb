#include <pebble.h>
#include "strdup.h"

char* strdup(const char* src){
	char *dest = malloc((strlen(src)+1)*sizeof(char));
  if (dest==NULL) return NULL;
	strcpy(dest, src);
	return dest;
}