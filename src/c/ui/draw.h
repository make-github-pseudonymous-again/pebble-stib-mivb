#pragma once
#include <pebble.h>
#include "../data/realtime.h"

// KO timeout
const time_t TKO = 60000;

void draw();
void draw_from_tm(struct tm *tick_time);
void draw_from_time(const time_t now);
void draw_realtime_item(Layer* root, const time_t now, const size_t i, const Realtime* realtime);

void clear();

void handle_error(const char* title , const char* message);
