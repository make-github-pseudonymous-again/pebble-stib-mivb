#include <pebble.h>
#include "click.h"
#include "next.h"
#include "load.h"
#include "scroll.h"

void click_config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_single_click_handler);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "single click select");
  next();
}

void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "long click select");
  load();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "single click down");
  scroll_down();
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "single click up");
  scroll_up();
}
