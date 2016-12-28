#pragma once
#include <pebble.h>

void main_window_load(Window *window);
void main_window_unload(Window *window);

GRect get_main_window_rect();
GSize get_main_window_size();
int16_t get_main_window_width();
int16_t get_main_window_height();

// to store the dimensions of the main window
static GRect s_rect;
static GSize s_size;
static int16_t s_width;
static int16_t s_height;
