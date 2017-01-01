#pragma once
#include <pebble.h>
#include "../data/realtime.h"

// KO timeout
#define TKO 60000

void draw(bool quiet);
void draw_from_tm(bool quiet, struct tm *tick_time);
void draw_from_time(bool quiet, const time_t now);
void draw_realtime_item(bool quiet, Layer* root, const time_t now, const size_t i, const Realtime* realtime);

void clear();

void handle_error(const char* title , const char* message);