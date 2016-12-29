#include <pebble.h>
#include "inbox.h"
#include "../data/realtime.h"
#include "../data/stop.h"
#include "../data/stops.h"
#include "../ui/draw.h"
#include "../ui/main_window.h"
#include "../ui/color.h"
#include "../pebble/translate_error.h"

#define VAL_TYPE_STATE 0
#define VAL_TYPE_REALTIME_STOP 1
#define VAL_TYPE_REALTIME_REALTIME 2
#define VAL_TYPE_REALTIME_END 3

#define VAL_STATE_LOADING 0
#define VAL_STATE_LOADED 1
#define VAL_STATE_LOADED_GEOERROR 2
#define VAL_STATE_LOADED_ERROR 3
#define VAL_STATE_RECV 4

time_t inbox_last_loaded_event_ts;

// used to keep track of last received message
static uint32_t s_uuid_run;
static uint32_t s_uuid_send_realtime;

void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  int size = (int)iterator->end - (int)iterator->dictionary;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[inbox] Received %d bytes", size);

  const uint32_t type = dict_find(iterator, MESSAGE_KEY_TYPE)->value->uint32;
  switch (type) {
    case VAL_TYPE_REALTIME_STOP: {
      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
      //const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;

      if ( uuid_run < s_uuid_run || ( uuid_run == s_uuid_run && uuid_send_realtime < s_uuid_send_realtime ) ) {
      	// this should  never happen
      	APP_LOG(APP_LOG_LEVEL_ERROR, "received old realtime message");
      	break;
      }

      if ( uuid_run > s_uuid_run || uuid_send_realtime > s_uuid_send_realtime ) {
      	// we are receiving new realtime data, drop old
      	APP_LOG(APP_LOG_LEVEL_DEBUG, "receiving new realtime data");
      	s_uuid_run = uuid_run;
      	s_uuid_send_realtime = uuid_send_realtime;
      	Stops_clear(&data_stops_recv);
      }

      const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
      const char *stop_name = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_NAME)->value->cstring;
      Tuple *stop_error_tuple = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ERROR);
      Tuple *stop_message_tuple = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_MESSAGE);
      uint32_t stop_error = 0;
      const char *stop_message = NULL;
      if (stop_error_tuple != NULL) {
        stop_error = stop_error_tuple->value->uint32;
        stop_message = "missing error message";
      }
      if (stop_message_tuple != NULL) stop_message = stop_message_tuple->value->cstring;

      Stop* stop = Stop_create(stop_id, stop_name, stop_error, stop_message);
      ds_DynamicArray_push(&data_stops_recv, stop);

      break;

    }
    case VAL_TYPE_REALTIME_REALTIME: {

      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
      //const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;

      if ( uuid_run != s_uuid_run || uuid_send_realtime != s_uuid_send_realtime ) {
      	// this should  never happen
      	APP_LOG(APP_LOG_LEVEL_ERROR, "received wrong realtime uuid");
      	break;
      }

      const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
      const char *line_number = dict_find(iterator, MESSAGE_KEY_REALTIME_LINE_NUMBER)->value->cstring;
      const char *destination_name = dict_find(iterator, MESSAGE_KEY_REALTIME_DESTINATION_NAME)->value->cstring;
      const uint32_t foreground_color = dict_find(iterator, MESSAGE_KEY_REALTIME_FOREGROUND_COLOR)->value->uint32;
      const uint32_t background_color = dict_find(iterator, MESSAGE_KEY_REALTIME_BACKGROUND_COLOR)->value->uint32;
      const uint32_t utc = dict_find(iterator, MESSAGE_KEY_REALTIME_UTC)->value->uint32;

      Realtime *realtime = Realtime_create(
      	stop_id,
      	line_number,
      	destination_name,
      	foreground_color,
      	background_color,
      	utc
      );

      Stop *stop = get_stop(&data_stops_recv, stop_id);

      if ( stop == NULL ) {
      	// this should  never happen
      	APP_LOG(APP_LOG_LEVEL_ERROR, "stop object %lu missing", stop_id);
      	break;
      }

      ds_DynamicArray_push(&stop->realtime, realtime);
      break;
    }
    case VAL_TYPE_REALTIME_END: {

      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;

      if ( uuid_run != s_uuid_run || uuid_send_realtime != s_uuid_send_realtime ) {
      	// this should  never happen
      	APP_LOG(APP_LOG_LEVEL_ERROR, "received wrong realtime uuid");
      	break;
      }
      
      // time to update the screen
      
      // swap stop lists
      ds_DynamicArray_swap(&data_stops_curr, &data_stops_recv);
      
      // search for most recently displayed stop
      ui_displayed_stop_index = 0; // default: show closest
      const size_t n = data_stops_curr.length;
      for (size_t i = 0 ; i < n ; ++i){
        Stop *stop = data_stops_curr.data[i];
        if (stop->id == ui_displayed_stop_id) {
          ui_displayed_stop_index = i;
          break;
        }
      }
      if (n > 0) {
        Stop *stop = data_stops_curr.data[ui_displayed_stop_index];
        ui_displayed_stop_id = stop->id;
      }

      clear();
      draw();
      Stops_clear(&data_stops_recv);
      break;
    }
    case VAL_TYPE_STATE: {
      const uint32_t state = dict_find(iterator, MESSAGE_KEY_STATE)->value->uint32;
      switch(state){
      	case VAL_STATE_LOADING:{
      	  status_bar_layer_set_colors(ui_status_bar, BLO, FLO);
      	  break;
        }
      	case VAL_STATE_LOADED:{
          inbox_last_loaded_event_ts = time(NULL);
      	  status_bar_layer_set_colors(ui_status_bar, BOK, FOK);
      	  break;
        }
      	case VAL_STATE_LOADED_GEOERROR:{
      	  status_bar_layer_set_colors(ui_status_bar, BNG, FNG);
      	  break;
        }
      	case VAL_STATE_LOADED_ERROR:{
      	  status_bar_layer_set_colors(ui_status_bar, BKO, FKO);
      	  break;
        }
      	case VAL_STATE_RECV:{
      	  status_bar_layer_set_colors(ui_status_bar, BRC, FRC);
      	  break;
        }
        default: {
          APP_LOG(APP_LOG_LEVEL_ERROR, "[inbox] Unknown state %lu", state);
        }
      }
      break;
    }
    default: {
      APP_LOG(APP_LOG_LEVEL_ERROR, "[inbox] Unknown type %lu", type);
    }
  }
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "[inbox] Message dropped: %i - %s", reason, pebble_translate_error(reason));
}