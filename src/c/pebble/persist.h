#pragma once
#include <pebble.h>

status_t persist_write_string_trunc(const uint32_t key, const char *cstring);
status_t persist_read_string_trunc(const uint32_t key, char *pt);