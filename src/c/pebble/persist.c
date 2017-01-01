#include <pebble.h>
#include "persist.h"

status_t persist_write_string_trunc(const uint32_t key, const char *cstring){
  const size_t n = strlen(cstring);
  const size_t m = n < PERSIST_DATA_MAX_LENGTH ? n : PERSIST_DATA_MAX_LENGTH;
  return persist_write_data(key, cstring, m);
}

status_t persist_read_string_trunc(const uint32_t key, char **pt){
  int size = persist_get_size(key);
  if (size == E_DOES_NOT_EXIST) return E_DOES_NOT_EXIST;
  *pt = malloc(size+1);
  status_t status = persist_read_data(key, *pt, size);
  (*pt)[size]='\0';
  return status;
}