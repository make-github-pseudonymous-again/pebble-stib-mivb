#pragma once
#include <pebble.h>

void click_config_provider(Window *window);

static void select_single_click_handler(ClickRecognizerRef recognizer, Window *window);
static void select_long_click_handler(ClickRecognizerRef recognizer, Window *window);
static void down_single_click_handler(ClickRecognizerRef recognizer, Window *window);
static void up_single_click_handler(ClickRecognizerRef recognizer, Window *window);
