#pragma once
#include <pebble.h>
#include "main_window.h"
#include "when.h"
#include "../ds/dynamicarray.h"

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

const char *MESSAGE_NOTHING = "nothing right now";

// GLOBALS
uint32_t ui_last_seen_stop_id;

// memory necessary to display realtime
char ui_minutes_buffer[DISPLAYED_ITEMS][WHEN_BUFFER_SIZE];

// dynamically allocated and freed memory for layers displaying realtime
TextLayer *ui_line_number_layer[DISPLAYED_ITEMS] = { NULL };
TextLayer *ui_destination_name_layer[DISPLAYED_ITEMS] = { NULL };
TextLayer *ui_minutes_layer[DISPLAYED_ITEMS] = { NULL };
