#include <pebble.h>
#include "ui/main_window.h"
#include "ui/color.h"

// to store the dimensions of the main window
static GRect s_rect;
static GSize s_size;
static int16_t s_width;
static int16_t s_height;

GRect get_main_window_rect(){
  return s_rect;
}

GSize get_main_window_size(){
  return s_size;
}

int16_t get_main_window_width(){
  return s_width;
}

int16_t get_main_window_height(){
  return s_height;
}

void main_window_load(Window *window) {

  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  s_rect = layer_get_bounds(window_layer);

  s_size = s_rect.size ;
  s_width = s_size.w ;
  s_height = s_size.h ;
  s_left = MARGIN_LEFT;
  s_top = MARGIN_TOP;

  // Set background color
  window_set_background_color(window, BG);

  // Create the StatusBarLayer
  s_status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(s_status_bar, BKO, FKO);
  status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeNone);
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

  // Stop name text layer
  s_stop_name_layer = text_layer_create(GRect(25, 0, s_width-50, 20));
  text_layer_set_font(s_stop_name_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(s_stop_name_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_stop_name_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text_color(s_stop_name_layer, GColorBlack);

  // Message text layer
  s_message_layer = text_layer_create(GRect(s_left, s_top, s_width - 34, s_height - s_top));
  text_layer_set_font(s_stop_name_layer, fonts_get_system_font(FONT));
  text_layer_set_text_alignment(s_stop_name_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_stop_name_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_color(s_stop_name_layer, BKO);

  // Update display with cached information
  draw();

  // Register click events
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

}

void main_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload");
  status_bar_layer_destroy(s_status_bar);
  text_layer_destroy(s_stop_name_layer);
  text_layer_destroy(s_message_layer);
  // TODO cache data on the watch
}
