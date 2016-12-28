#pragma once
#include <pebble.h>
#include "../data/realtime.h"

void draw();
void draw_from_tm(struct tm *tick_time);
void draw_from_time(const time_t now);
void draw_realtime_item(const time_t now, const size_t i, const Realtime* realtime);

void clear();

void handle_error(const char* title , const char* message);
