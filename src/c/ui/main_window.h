#pragma once
#include <pebble.h>

#define DISPLAYED_ITEMS 2

// initialized once, deleted on app kill
Window *ui_main_window = NULL;
// initialized once, deleted on app kill
StatusBarLayer *ui_status_bar = NULL;
// initialized once, deleted on app kill
TextLayer *ui_info_layer = NULL;
// initialized once, deleted on app kill
TextLayer *ui_stop_name_layer = NULL;
// initialized once, deleted on app kill
TextLayer *ui_message_layer = NULL;

// used to remember which stop we are displaying
uint32_t ui_displayed_stop_id = 0;
size_t ui_displayed_stop_index = 0;

void main_window_load(Window *window);
void main_window_unload(Window *window);

GRect get_main_window_rect();
GSize get_main_window_size();
int16_t get_main_window_width();
int16_t get_main_window_height();