#pragma once
#include <pebble.h>

void click_config_provider(Window *window);

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window);
void select_long_click_handler(ClickRecognizerRef recognizer, Window *window);
void down_single_click_handler(ClickRecognizerRef recognizer, Window *window);
void up_single_click_handler(ClickRecognizerRef recognizer, Window *window);
void select_double_click_handler(ClickRecognizerRef recognizer, Window *window);