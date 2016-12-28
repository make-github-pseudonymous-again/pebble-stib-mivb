#pragma once
#include <pebble.h>
#include "when.h"

// TODO make those dimensions depend on the watch model
const int16_t BOXHEIGHT = 32;
const int16_t MARGIN = 3;
const int16_t LINEHEIGHT = BOXHEIGHT + MARGIN;

const int16_t MARGIN_LEFT = 15;
const int16_t MARGIN_TOP = 30;

// font
const char *FONT = FONT_KEY_GOTHIC_24_BOLD;

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

// message displayed when no information is available at the current stop
const char *MESSAGE_NOTHING = "nothing right now";

// used to remember which stop we are displaying
uint32_t ui_displayed_stop_id = 0;
size_t ui_displayed_stop_index = 0;

// memory necessary to display realtime
char ui_minutes_buffer[DISPLAYED_ITEMS][WHEN_BUFFER_SIZE];

// dynamically allocated and freed memory for layers displaying realtime
TextLayer *ui_line_number_layer[DISPLAYED_ITEMS] = { NULL };
TextLayer *ui_destination_name_layer[DISPLAYED_ITEMS] = { NULL };
TextLayer *ui_minutes_layer[DISPLAYED_ITEMS] = { NULL };

// load and unload handlers for main window
void main_window_load(Window *window);
void main_window_unload(Window *window);

// retrieve window dimensions
GRect get_main_window_rect();
GSize get_main_window_size();
int16_t get_main_window_width();
int16_t get_main_window_height();
int16_t get_main_window_left();
int16_t get_main_window_top();