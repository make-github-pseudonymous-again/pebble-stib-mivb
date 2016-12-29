#pragma once
#include <pebble.h>
#include "when.h"

// TODO make those dimensions depend on the watch model
#if defined(PBL_PLATFORM_APLITE)
  #define DISPLAYED_ITEMS 4
  #define SCROLL_STEP 3
  #define BOXHEIGHT 32
  #define BOXWIDTH 32
  #define MARGIN 3
  #define LINEHEIGHT (BOXHEIGHT + MARGIN)
  
  #define MARGIN_LEFT 5
  #define MARGIN_TOP 20
  #define TITLE_HEIGHT 25
  #define MINUTES_WIDTH 22
#else
#if defined(PBL_PLATFORM_BASALT)
  #define DISPLAYED_ITEMS 4
  #define SCROLL_STEP 3
  #define BOXHEIGHT 32
  #define BOXWIDTH 32
  #define MARGIN 3
  #define LINEHEIGHT (BOXHEIGHT + MARGIN)
  
  #define MARGIN_LEFT 5
  #define MARGIN_TOP 20
  #define TITLE_HEIGHT 25
  #define MINUTES_WIDTH 22
#else
#if defined(PBL_PLATFORM_DIORITE)
  #define DISPLAYED_ITEMS 4
  #define SCROLL_STEP 3
  #define BOXHEIGHT 32
  #define BOXWIDTH 32
  #define MARGIN 3
  #define LINEHEIGHT (BOXHEIGHT + MARGIN)
  
  #define MARGIN_LEFT 5
  #define MARGIN_TOP 20
  #define TITLE_HEIGHT 25
  #define MINUTES_WIDTH 22
#else
#if defined(PBL_PLATFORM_EMERY)
  #define DISPLAYED_ITEMS 6
  #define SCROLL_STEP 5
  #define BOXHEIGHT 32
  #define BOXWIDTH 32
  #define MARGIN 3
  #define LINEHEIGHT (BOXHEIGHT + MARGIN)
  
  #define MARGIN_LEFT 5
  #define MARGIN_TOP 20
  #define TITLE_HEIGHT 25
  #define MINUTES_WIDTH 22
#else // PBL_PLATFORM_CHALK + default
  #define DISPLAYED_ITEMS 4
  #define SCROLL_STEP 3
  #define BOXHEIGHT 32
  #define BOXWIDTH 32
  #define MARGIN 3
  #define LINEHEIGHT (BOXHEIGHT + MARGIN)
  
  #define MARGIN_LEFT 15
  #define MARGIN_TOP 25
  #define TITLE_HEIGHT 25
  #define MINUTES_WIDTH 22
#endif
#endif
#endif
#endif

// font
#define FONT FONT_KEY_GOTHIC_24_BOLD

extern Window *ui_main_window;
extern StatusBarLayer *ui_status_bar;
extern TextLayer *ui_info_layer;
extern TextLayer *ui_stop_name_layer;
extern TextLayer *ui_message_layer;
extern const char *MESSAGE_NOTHING;
extern uint32_t ui_displayed_stop_id;
extern size_t ui_displayed_stop_index;
extern char ui_minutes_buffer[DISPLAYED_ITEMS][WHEN_BUFFER_SIZE];
extern TextLayer *ui_line_number_layer[DISPLAYED_ITEMS];
extern TextLayer *ui_destination_name_layer[DISPLAYED_ITEMS];
extern TextLayer *ui_minutes_layer[DISPLAYED_ITEMS];

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
int16_t get_main_window_title();