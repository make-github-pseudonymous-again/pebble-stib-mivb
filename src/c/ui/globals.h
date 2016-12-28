#pragma once
#include <pebble.h>

// CONSTANTS

// KO timeout
const time_t TKO = 60000;

const int16_t BOXHEIGHT = 32;
const int16_t MARGIN = 3;
const int16_t LINEHEIGHT = BOXHEIGHT + MARGIN;

const int16_t MARING_LEFT = 15;
const int16_t MARING_TOP = 30;

// font
const char *FONT = FONT_KEY_GOTHIC_24_BOLD;

const size_t DISPLAYED_ITEMS = 2;
const char *MESSAGE_NOTHING = "nothing right now";

// GLOBALS
uint32_t ui_last_seen_stop_id;
// list with displayed data
ds_DynamicArray ui_stops;
// list with data being received
ds_DynamicArray ui_stops_recv;

// memory necessary to display realtime
char ui_minutes_buffer[DISPLAYED_ITEMS][WHEN_BUFFER_SIZE];

// dynamically allocated and freed memory for layers displaying realtime
TextLayer *ui_line_number_layer[DISPLAYED_ITEMS] = { NULL };
TextLayer *ui_destination_name_layer[DISPLAYED_ITEMS] = { NULL };
TextLayer *ui_minutes_layer[DISPLAYED_ITEMS] = { NULL };

// used to remember which stop we are displaying
uint32_t ui_displayed_stop_id = 0;
size_t ui_displayed_stop_index = 0;

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

// used to keep track of last received message
uint32_t ui_uuid_run;
uint32_t ui_uuid_send_realtime;
