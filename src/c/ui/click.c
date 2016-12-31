#include <pebble.h>
#include "click.h"
#include "show.h"
#include "load.h"
#include "scroll.h"

void click_config_provider(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[click] config_provider");
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_single_click_handler);
  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 0, 300, false, (ClickHandler) select_double_click_handler);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[click] single select");
  show_next();
}

void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[click] long select");
  load();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[click] single down");
  scroll_down();
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[click] single up");
  scroll_up();
}

void select_double_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[click] double select");
  show_closest();
}